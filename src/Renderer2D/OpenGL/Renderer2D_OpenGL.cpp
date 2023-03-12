#include "Renderer2D_OpenGL.h"

#include "Core/Logger.h"
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

#define PRIMITIVE_RESTART_INDEX 0xFFFFFFFF

// TODO
// https://bcmpinc.wordpress.com/2015/08/18/creating-an-opengl-4-5-context-using-sdl2-and-glad/

static bool areGLProcsLoaded = false;
static const int maxTexturesBind = 4;

/*
	0 ------ 3
	|        |
	|        |
	1 ------ 2
*/

static const std::vector<Vertex> quadVertices = {
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
	// vec2 position = inPosition;
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
	// FragColor = color;
}
)";

static Renderer2D_OpenGL::Texture LoadTextureToGPU(const char* path)
{
	int width, height, nComponents;
	unsigned char* imageData = stbi_load(path, &width, &height, &nComponents, 0);
	if (!imageData)
	{
		// cannot load image...
		LOG_ERROR("unable to load image at: " + std::string(path));
		stbi_image_free(imageData);

		return {(unsigned int)-1, {-1, -1}};
	}

	GLenum format = GL_RGB;
	if (nComponents == 1)
		format = GL_RED;
	else if (nComponents == 3)
		format = GL_RGB;
	else if (nComponents == 4)
		format = GL_RGBA;

	unsigned int texture = -1;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(imageData);

	const size_t maxMsgSz = 500;
	char message[maxMsgSz] = { 0 };
	snprintf(message, maxMsgSz, "loaded %s: %d", path, texture);
	LOG_INFO(message);

	return { texture, { width, height }, std::string(path) };
}

static Renderable CreateRenderable(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	Renderable renderable;
	glGenVertexArrays(1, &renderable.VAO);
	glBindVertexArray(renderable.VAO);

	glGenBuffers(1, &renderable.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STREAM_DRAW);

	glGenBuffers(1, &renderable.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	renderable.nElements = indices.size();

	return renderable;
}

static void DestroyRenderable(Renderable& renderable)
{
	glDeleteBuffers(1, &renderable.VBO);
	glDeleteBuffers(1, &renderable.EBO);
	glDeleteBuffers(1, &renderable.VAO);
}

static int CreateShader(const char* v_shader, const char* v_fragment)
{
	int id = -1;

	// const char* v_shader = vertexSrc.c_str();
	// const char* v_fragment = fragmentSrc.c_str();

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
		LOG_ERROR(std::string("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n") + infoLog);
	}
	// 2. fragment shader
	unsigned int shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shaderFragment, 1, &v_fragment, nullptr);
	glCompileShader(shaderFragment);
	glGetShaderiv(shaderFragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderFragment, sizeof(infoLog), nullptr, infoLog);
		LOG_ERROR(std::string("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n") + infoLog);
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
		LOG_ERROR(std::string("ERROR::SHADER::PROGRAM::LINK_FAILED\n") + infoLog);
	}

	glDeleteShader(shaderVertex);
	glDeleteShader(shaderFragment);
	return id;
}

void Renderer2D_OpenGL::LoadOpenGLLibrary(void*(procAddr)(const char*))
{
	if (!areGLProcsLoaded)
	{
		gladLoadGLLoader(procAddr);
		areGLProcsLoaded = true;
	}
}


Renderer2D_OpenGL::Renderer2D_OpenGL()
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
			LOG_ERROR("unable to find quadTextureCoords[" + std::to_string(i) + "] uniform location.");
		}
	}

	if (m_uniformOffsetLoc < 0)
		LOG_ERROR("unable to find offset uniform location.");

	if (m_uniformScaleLoc < 0)
		LOG_ERROR("unable to find scale uniform location.");

	if (m_uniformTextureIdLoc < 0)
		LOG_ERROR("unable to find textureId uniform location.");

	m_quadRenderable = CreateRenderable(quadVertices, quadIndices);

	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glEnable(GL_BLEND);
	// glEnable(GL_PRIMITIVE_RESTART);
	// glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
}

Renderer2D_OpenGL::~Renderer2D_OpenGL()
{
	DestroyRenderable(m_quadRenderable);
	for (auto& texture : m_textures)
	{
		glDeleteTextures(1, &texture.id);
	}
	glDeleteProgram(m_shader);
}

void Renderer2D_OpenGL::Begin()
{
	glUseProgram(m_shader);
	glBindVertexArray(m_quadRenderable.VAO);
	// for (int i = 0; i < m_textures.size() && i < maxTexturesBind; ++i)
	// {
	// 	glActiveTexture(GL_TEXTURE0 + i);
	// 	glBindTexture(GL_TEXTURE0 + i, m_textures[i].id);
	// }
}

void Renderer2D_OpenGL::End()
{
	Flush();
	m_onRenderEnd();
}

void Renderer2D_OpenGL::Flush()
{
}

void Renderer2D_OpenGL::Clear(const Color& color) const
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/*
void Renderer2D_OpenGL::DrawDisk(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL::FillCircle(const pt2di& center, int radius, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_OpenGL::FillRect(const pt2di& position, const vec2di& dimensions, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawLine(const pt2di& start, const pt2di& end, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawText(const pt2di& position, const std::string& text, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawText(const pt2di& topLeft, const vec2di& dimensions, const std::string& text, const Color& color) const
{

}

void Renderer2D_OpenGL::DrawArrow(const pt2di& start, const pt2di& end, const Color& color) const
{

}
*/


Renderer2D::SpriteID Renderer2D_OpenGL::LoadTexture(const char* path, const std::string& tag)
{
	Texture texture = ::LoadTextureToGPU(path);
	m_textures.push_back(texture);
	size_t textureId = m_textures.size() - 1;
	m_sprites.push_back({ textureId, { 0.0f, 0.0f }, { 1.0f, 1.0f } });
	SpriteID spriteId = m_sprites.size() - 1;
	m_tagsMap.emplace(tag, spriteId);

	return spriteId;
}

std::vector<Renderer2D::SpriteID> Renderer2D_OpenGL::LoadSpriteSheet(const char* path, const std::vector<SpriteDescriptor>& spriteDescriptors)
{
	Texture texture = ::LoadTextureToGPU(path);
	m_textures.push_back(texture);
	size_t textureId = m_textures.size() - 1;
	std::vector<SpriteID> sprites;

	float textureWidth = (float)texture.dim.w;
	float textureHeight = (float)texture.dim.h;

	for (const auto& desc : spriteDescriptors)
	{
		Sprite sprite;
		sprite.textureId = textureId;

		pt2di topLeft = desc.offset;
		switch (desc.offsetType)
		{
		case SpriteOffsetType::TopLeft:
			break;

		case SpriteOffsetType::TopRight:
			topLeft.x -= desc.size.w;
			break;

		case SpriteOffsetType::BottomLeft:
			topLeft.y -= desc.size.h;
			break;

		case SpriteOffsetType::BottomRight:
			topLeft = desc.offset - desc.size;
			break;
		
		default:
			LOG_ERROR("unknown SpriteOffsetType.");
			continue;
		}

		pt2di bottomRight = topLeft + desc.size;

		sprite.topLeft = { (float)topLeft.x / textureWidth, (float)topLeft.y / textureHeight };
		sprite.bottomRight = { (float)bottomRight.x / textureWidth, (float)bottomRight.y / textureHeight };

		m_sprites.push_back(sprite);
		SpriteID spriteId = m_sprites.size() - 1;
		m_tagsMap.emplace(desc.tag, spriteId);
	}

	return sprites;
}

bool Renderer2D_OpenGL::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const SpriteID& spriteId) const
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
	Texture texture = m_textures[sprite.textureId];

	pt2df normalizedPosition = normalizeCoords(topLeft, m_viewPortDim);
	vec2df normalizedSize = normalizeSize(dimensions, m_viewPortDim);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE0, texture.id);

	glUniform2f(m_uniformOffsetLoc, normalizedPosition.x, normalizedPosition.y);
	glUniform2f(m_uniformScaleLoc, normalizedSize.w, normalizedSize.h);
	// glUniform1i(m_uniformTextureIdLoc, sprite.textureId);
	glUniform1i(m_uniformTextureIdLoc, 0);

	for (int i = 0; i < N_QUAD_VERTICES; ++i)
	{
		pt2df textureCoord = {  sprite.topLeft.x * quadWeights[i][0] + sprite.bottomRight.x * quadWeights[i][2],
								sprite.topLeft.y * quadWeights[i][1] + sprite.bottomRight.y * quadWeights[i][3] };
		const size_t uniformStrSz = 50;
		char uniform[uniformStrSz] = { 0 };
		snprintf(uniform, uniformStrSz, "quadTextureCoords[%d]", i);
		glUniform2f(m_uniformQuadTextureCoordsLoc[i], textureCoord.x, textureCoord.y);
	}

	glDrawElements(GL_TRIANGLES, m_quadRenderable.nElements, GL_UNSIGNED_INT, (void*)0);

	// send the texture as uniform
	// send the texture coords as uniform

	return true;
}

bool Renderer2D_OpenGL::DrawSprite(const pt2di& topLeft, const vec2di& dimensions, const std::string& tag) const
{
	try
	{
		const SpriteID& spriteId = m_tagsMap.at(tag);
		return DrawSprite(topLeft, dimensions, spriteId);
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("an error has occured with tag: " + tag + " - " + e.what());
	}
	return false;
}


vec2di Renderer2D_OpenGL::GetSpriteSize(const std::string& spriteTag) const
{
	return {};
}

// const vec2di& Renderer2D_OpenGL::GetWindowDim() const
// {
// 	return m_windowDim;
// }

const vec2di& Renderer2D_OpenGL::GetCellDim() const
{
	return m_cellDim;
}

const vec2di& Renderer2D_OpenGL::GetViewPortDim() const
{
	return m_viewPortDim;
}

// void Renderer2D_OpenGL::SetWindowDim(const vec2di& dim)
// {
// 	m_windowDim = dim;
// 	SetViewPortDim(m_windowDim);
// }

void Renderer2D_OpenGL::SetCellDim(const vec2di& dim)
{
	m_cellDim = dim;
}

void Renderer2D_OpenGL::SetViewPortDim(const vec2di& dim)
{
	// TODO recalculate projection matrix
	// TODO set the view port
	m_viewPortDim = dim;
	glViewport(0, 0, m_viewPortDim.w, m_viewPortDim.h);
}
