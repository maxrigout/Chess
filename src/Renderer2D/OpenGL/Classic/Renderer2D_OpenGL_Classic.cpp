#include "Renderer2D_OpenGL_Classic.h"

#include "Core/Logger.h"
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

#define PRIMITIVE_RESTART_INDEX 0xFFFFFFFF

// #define PREBIND_TEXTURES

// TODO
// https://bcmpinc.wordpress.com/2015/08/18/creating-an-opengl-4-5-context-using-sdl2-and-glad/

/*
	0 ------ 3
	|        |
	|        |
	1 ------ 2
*/

static const std::vector<Renderer2D_OpenGL_Classic::Vertex> quadVertices = {
	{ { 0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
	{ { 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
	{ { 1.0f,  0.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
};

// with TopLeft of window as origin
// static const std::vector<Vertex> quadVertices = {
// 	{ { -1.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
// 	{ { -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
// 	{ {  0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
// 	{ {  0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } }
// };

static const std::vector<unsigned int> quadIndices {
	0, 1, 2,
	0, 2, 3
};

//   tl_x, tl_y, br_x, br_y
static const float quadWeights[N_QUAD_VERTICES][4] = {
	{ 1, 1, 0, 0 },
	{ 1, 0, 0, 1 },
	{ 0, 0, 1, 1 },
	{ 0, 1, 1, 0 },
};

static const char* vertexSource = R"(
#version 410 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec4 inColor;

uniform vec2 offset;
uniform vec2 scale;
uniform vec2 quadTextureCoords[4];

out vec4 color;
out vec2 uv;

void main()
{
	vec2 position = inPosition * scale + offset;
	gl_Position = vec4(position, 0.0, 1.0);
	uv = quadTextureCoords[gl_VertexID];
	color = inColor;
}
)";

static const char* fragmentSource = R"(
#version 410 core

uniform sampler2D textureId;

in vec4 color;
in vec2 uv;

out vec4 FragColor;

void main()
{
	FragColor = texture(textureId, uv);
}
)";

static Renderer2D_OpenGL_Classic::Texture LoadTextureToGPU(const char* path)
{
	int width, height, nComponents;
	uint8_t* imageData = stbi_load(path, &width, &height, &nComponents, 0);
	if (!imageData)
	{
		// cannot load image...
		LOG_ERROR("unable to load image at: ", path);
		//stbi_image_free(imageData);

		return {(unsigned int)-1, {-1, -1}};
	}

	GLenum format = GL_RGB;
	if (nComponents == 1)
		format = GL_RED;
	else if (nComponents == 3)
		format = GL_RGB;
	else if (nComponents == 4)
		format = GL_RGBA;

	unsigned int tex = -1;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(imageData);

	const size_t maxMsgSz = 500;
	char message[maxMsgSz] = { 0 };
	snprintf(message, maxMsgSz, "loaded %s: %d", path, tex);
	LOG_INFO(message);

	return { tex, { width, height }, std::string(path) };
}

static Renderer2D_OpenGL_Classic::Renderable CreateRenderable(const std::vector<Renderer2D_OpenGL_Classic::Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	Renderer2D_OpenGL_Classic::Renderable renderable;
	glGenVertexArrays(1, &renderable.VAO);
	glBindVertexArray(renderable.VAO);

	glGenBuffers(1, &renderable.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Renderer2D_OpenGL_Classic::Vertex), vertices.data(), GL_STREAM_DRAW);

	glGenBuffers(1, &renderable.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer2D_OpenGL_Classic::Vertex), (void*)offsetof(Renderer2D_OpenGL_Classic::Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Renderer2D_OpenGL_Classic::Vertex), (void*)offsetof(Renderer2D_OpenGL_Classic::Vertex, color));

	renderable.nElements = indices.size();

	return renderable;
}

static void DestroyRenderable(Renderer2D_OpenGL_Classic::Renderable& renderable)
{
	glDeleteBuffers(1, &renderable.VBO);
	glDeleteBuffers(1, &renderable.EBO);
	glDeleteBuffers(1, &renderable.VAO);
}

static int CreateShader(const char* v_shader, const char* v_fragment)
{
	int id = -1;

	int success;
	const int infoLogSz = 512;
	char infoLog[infoLogSz];

	// create and compile the shaders
	// 1. vertex shader
	unsigned int shaderVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shaderVertex, 1, &v_shader, nullptr);
	glCompileShader(shaderVertex);
	glGetShaderiv(shaderVertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderVertex, sizeof(infoLog), nullptr, infoLog);
		LOG_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n", infoLog);
	}
	// 2. fragment shader
	unsigned int shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &v_fragment, nullptr);
	glCompileShader(shaderFragment);
	glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderFragment, sizeof(infoLog), nullptr, infoLog);
		LOG_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n", infoLog);
	}

	// create the shader progam
	id = glCreateProgram();
	glAttachShader(id, shaderVertex);
	glAttachShader(id, shaderFragment);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog);
		LOG_ERROR("ERROR::SHADER::PROGRAM::LINK_FAILED\n", infoLog);
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFragment);
	return id;
}

Renderer2D_OpenGL_Classic::Renderer2D_OpenGL_Classic()
{
	m_onRenderEnd = [](){};
	m_shader = CreateShader(vertexSource, fragmentSource);

	m_uniformOffsetLoc = glGetUniformLocation(m_shader, "offset");
	m_uniformScaleLoc = glGetUniformLocation(m_shader, "scale");
	m_uniformTextureIdLoc = glGetUniformLocation(m_shader, "textureId");

	for (int i = 0; i < N_QUAD_VERTICES; ++i)
	{
		const size_t uniformStrSz = 50;
		char uniform[uniformStrSz] = { 0 };
		snprintf(uniform, uniformStrSz, "quadTextureCoords[%d]", i);
		m_uniformQuadTextureCoordsLoc[i] = glGetUniformLocation(m_shader, uniform);
		if (m_uniformQuadTextureCoordsLoc[i] < 0)
		{
			LOG_ERROR("unable to find quadTextureCoords[", i, "] uniform location.");
		}
	}

	if (m_uniformOffsetLoc < 0)
		LOG_ERROR("unable to find offset uniform location.");

	if (m_uniformScaleLoc < 0)
		LOG_ERROR("unable to find scale uniform location.");

	if (m_uniformTextureIdLoc < 0)
		LOG_ERROR("unable to find textureId uniform location.");

	m_quadRenderable = CreateRenderable(quadVertices, quadIndices);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Renderer2D_OpenGL_Classic::~Renderer2D_OpenGL_Classic()
{
	DestroyRenderable(m_quadRenderable);
	for (auto& [path, texture] : m_textures)
	{
		glDeleteTextures(1, &texture.id);
	}
	glDeleteProgram(m_shader);
}

void Renderer2D_OpenGL_Classic::Begin()
{
	glUseProgram(m_shader);
	glBindVertexArray(m_quadRenderable.VAO);
#ifdef PREBIND_TEXTURES
	for (int i = 0; i < m_textures.size() && i < maxTexturesBind; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
#endif
}

void Renderer2D_OpenGL_Classic::End()
{
	Flush();
	m_onRenderEnd();
}

void Renderer2D_OpenGL_Classic::Flush()
{
	
}

Renderer2D_OpenGL_Classic::Texture& Renderer2D_OpenGL_Classic::GetTexture(const std::string& path)
{
	if (m_textures.count(path) == 0)
	{
		Texture t = LoadTextureToGPU(path.c_str());
		m_textures.emplace(path, t);
	}
	return m_textures[path];
}

void Renderer2D_OpenGL_Classic::Clear(const Color& color) const
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
void Renderer2D_OpenGL_Classic::DrawDisk(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::FillCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawLine(const pt2di& start, const pt2di& end, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawText(const pt2di& position, const std::string& text, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const
{

}

void Renderer2D_OpenGL_Classic::DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const
{

}
*/

Renderer2D::SpriteID Renderer2D_OpenGL_Classic::LoadSprite(const SpriteDescriptor& spriteDescriptor)
{
	Texture texture = GetTexture(spriteDescriptor.texturePath);
	Sprite sprite;
	sprite.textureId = spriteDescriptor.texturePath;
	float textureWidth = (float)texture.dim.w;
	float textureHeight = (float)texture.dim.h;

	pt2di topLeft = spriteDescriptor.offset;
	vec2di effectiveSize = spriteDescriptor.size;
	if (spriteDescriptor.size.x == -1)
	{
		effectiveSize.w = texture.dim.w;
	}

	if (spriteDescriptor.size.y == -1)
	{
		effectiveSize.h = texture.dim.h;
	}
	switch (spriteDescriptor.offsetType)
	{
		case SpriteOffsetType::TopLeft:
			break;

		case SpriteOffsetType::TopRight:
			topLeft.x -= effectiveSize.w;
			break;

		case SpriteOffsetType::BottomLeft:
			topLeft.y -= effectiveSize.h;
			break;

		case SpriteOffsetType::BottomRight:
			topLeft = spriteDescriptor.offset - effectiveSize;
			break;

		default:
			LOG_ERROR("unknown SpriteOffsetType.");
	}

	pt2di bottomRight = topLeft + effectiveSize;

	sprite.topLeft = { (float)topLeft.x / textureWidth, (float)topLeft.y / textureHeight };
	sprite.bottomRight = { (float)bottomRight.x / textureWidth, (float)bottomRight.y / textureHeight };

	m_sprites.push_back(sprite);
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(spriteDescriptor.tag, spriteId);
	return spriteId;
}


std::vector<Renderer2D::SpriteID> Renderer2D_OpenGL_Classic::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	std::vector<SpriteID> sprites(spriteDescriptors.size());

	for (const auto& desc : spriteDescriptors)
	{
		sprites.push_back(LoadSprite(desc));
	}

	return sprites;
}

Renderer2D::SpriteID Renderer2D_OpenGL_Classic::LoadTexture(const char* path, const std::string& tag)
{
	Texture texture = LoadTextureToGPU(path);
	m_textures.emplace(path, texture);
	m_sprites.push_back({ path, { 0.0f, 0.0f }, { 1.0f, 1.0f } });
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(tag, spriteId);

	return spriteId;
}

bool Renderer2D_OpenGL_Classic::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const
{
	if (!IsValidSpriteId(spriteId))
		return false;

	const auto normalizeCoords = [] (const pt2di& coord, const vec2di& viewPortSize) -> pt2df
	{
		return { 2.0f * (float)coord.x / (float)viewPortSize.w - 1.0f, -2.0f * (float)coord.y / (float)viewPortSize.h + 1.0f };
	};

	const auto normalizeSize = [] (const vec2di& sz, const vec2di& viewPortSize) -> vec2df
	{
		return { 2.0f * (float)sz.w / (float)viewPortSize.w, 2.0f * (float)sz.h / (float)viewPortSize.h };
	};

	Sprite sprite = m_sprites[spriteId];
	Texture texture = m_textures.at(sprite.textureId);

	pt2df normalizedPosition = normalizeCoords(topLeft, m_viewPortDim);
	vec2df normalizedSize = normalizeSize(dimensions, m_viewPortDim);

	glUniform2f(m_uniformOffsetLoc, normalizedPosition.x, normalizedPosition.y);
	glUniform2f(m_uniformScaleLoc, normalizedSize.w, normalizedSize.h);
#ifdef PREBIND_TEXTURES
	glUniform1i(m_uniformTextureIdLoc, sprite.textureId);
#else
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glUniform1i(m_uniformTextureIdLoc, 0);
#endif

	for (int i = 0; i < N_QUAD_VERTICES; ++i)
	{
		pt2df textureCoord = {  sprite.topLeft.x * quadWeights[i][0] + sprite.bottomRight.x * quadWeights[i][2],
								sprite.topLeft.y * quadWeights[i][1] + sprite.bottomRight.y * quadWeights[i][3] };
		// const size_t uniformStrSz = 50;
		// char uniform[uniformStrSz] = { 0 };
		// snprintf(uniform, uniformStrSz, "quadTextureCoords[%d]", i);
		glUniform2f(m_uniformQuadTextureCoordsLoc[i], textureCoord.x, textureCoord.y);
	}

	glDrawElements(GL_TRIANGLES, m_quadRenderable.nElements, GL_UNSIGNED_INT, (void*)0);

	return true;
}

bool Renderer2D_OpenGL_Classic::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const
{
	try
	{
		const SpriteID& spriteId = m_tagsMap.at(tag);
		return DrawSprite(topLeft, dimensions, spriteId);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR_ONCE("an error has occured with tag: " + tag + " - " + e.what());
	}
	return false;
}


vec2di Renderer2D_OpenGL_Classic::GetSpriteSize(const std::string& spriteTag) const
{
	return {};
}

const vec2di& Renderer2D_OpenGL_Classic::GetCellDim() const
{
	return m_cellDim;
}

const vec2di& Renderer2D_OpenGL_Classic::GetViewPortDim() const
{
	return m_viewPortDim;
}

void Renderer2D_OpenGL_Classic::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}

void Renderer2D_OpenGL_Classic::SetViewPortDim(const vec2di& dim)
{
	m_viewPortDim = dim;
	glViewport(0, 0, m_viewPortDim.w, m_viewPortDim.h);
}
