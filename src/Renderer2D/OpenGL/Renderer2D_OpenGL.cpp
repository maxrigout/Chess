#include "Renderer2D_OpenGL.h"

#include "Core/Logger.h"
#include <iostream>

#include <glad/glad.h>
#include <stb/stb_image.h>

// TODO
// https://bcmpinc.wordpress.com/2015/08/18/creating-an-opengl-4-5-context-using-sdl2-and-glad/

static bool areGLProcsLoaded = false;
static const size_t maxInstances = 1000;
static const int maxTexturesBind = 4;

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

static const char* vertexSource = R"(
#version 410 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec4 inColor;
layout (location = 2) in vec2 inOffset;
layout (location = 3) in vec2 inScale;
layout (location = 4) in vec2 inTextureCoord;

out vec4 color;
out vec2 uv;

void main()
{

	vec2 position = (inPosition * inScale) + inOffset;
	// vec2 position = inPosition + inOffset;
	gl_Position = vec4(position, 0.0, 1.0);
	uv = inTextureCoord;
	color = inColor;
}
)";

static const char* fragmentSource = R"(
#version 410 core

in vec4 color;
in vec2 uv;

uniform sampler2D textureId;

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

	return { texture, { width, height } };
}

static Renderable CreateRenderable(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	Renderable renderable;
	glGenVertexArrays(1, &renderable.VAO);
	glBindVertexArray(renderable.VAO);

	glGenBuffers(1, &renderable.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &renderable.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderable.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	renderable.nElements = indices.size();

	return renderable;
}

template <typename T>
static unsigned int CreateInstanceVBO(int index, int nComponents, int maxSize)
{
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(T) * maxSize, nullptr, GL_STREAM_DRAW);

	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, nComponents, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);
	glVertexAttribDivisor(index, 1);

	return instanceVBO;
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
	m_uniformTextureIdLoc = glGetUniformLocation(m_shader, "textureId");

	if (m_uniformTextureIdLoc < 0)
		LOG_ERROR("unable to find textureId uniform location");

	m_quadRenderable = CreateRenderable(quadVertices, quadIndices);

	// Create instance VBO
	m_instanceVBO1 = CreateInstanceVBO<pt2df>(2, 2, maxInstances);
	m_instanceVBO2 = CreateInstanceVBO<vec2df>(3, 2, maxInstances);
	m_instanceVBO3 = CreateInstanceVBO<vec2df>(4, 2, maxInstances);

	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
}

Renderer2D_OpenGL::~Renderer2D_OpenGL()
{
	glDeleteBuffers(1, &m_instanceVBO2);
	glDeleteBuffers(1, &m_instanceVBO1);
	DestroyRenderable(m_quadRenderable);
	for (const auto& texture : m_textures)
	{
		glDeleteTextures(1, &texture.id);
	}
	glDeleteProgram(m_shader);
}

void Renderer2D_OpenGL::Begin()
{
	m_nInstances = 0;
	m_boundTexture = -1;
	m_texturesBound.clear();
	m_instanceScales.clear();
	m_instanceOffsets.clear();

	glUseProgram(m_shader);
	glBindVertexArray(m_quadRenderable.VAO);
}

void Renderer2D_OpenGL::End()
{
	Flush();
	m_onRenderEnd();
}

void Renderer2D_OpenGL::Flush()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO1);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pt2df) * m_instanceOffsets.size(), m_instanceOffsets.data());

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO2);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2df) * m_instanceScales.size(), m_instanceScales.data());

	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO3);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2df) * m_instanceTextureCoords.size(), m_instanceTextureCoords.data());

	glBindVertexArray(m_quadRenderable.VAO);
	glDrawElementsInstanced(GL_TRIANGLES, m_quadRenderable.nElements, GL_UNSIGNED_INT, 0, m_nInstances);

	m_nInstances = 0;
	m_boundTexture = -1;
	m_texturesBound.clear();
	m_instanceScales.clear();
	m_instanceOffsets.clear();
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
		return { 2.0f * (float)coord.x / (float)viewPortSize.w, -2.0f * (float)coord.y / (float)viewPortSize.h };
	};

	const auto normalizeSize = [] (const vec2di& sz, const vec2di& viewPortSize) -> vec2df
	{
		return { 2.0f * (float)sz.w / (float)viewPortSize.w, 2.0f * (float)sz.h / (float)viewPortSize.h };
	};

	Sprite sprite = m_sprites[spriteId];
	Texture texture = m_textures[sprite.textureId];
	pt2df normalizedPosition = normalizeCoords(topLeft, m_viewPortDim);
	vec2df normalizedSize = normalizeSize(dimensions, m_viewPortDim);
	if (!m_texturesBound.count(sprite.textureId) && m_boundTexture < maxTexturesBind)
	{
		unsigned int gl_textureID = GL_TEXTURE0 + ++m_boundTexture;
		glActiveTexture(gl_textureID);
		glBindTexture(gl_textureID, sprite.textureId);
	}
	glUniform1i(m_uniformTextureIdLoc, m_boundTexture);

	m_instanceOffsets.push_back(normalizedPosition + vec2df{ -1.0f, 1.0f });
	m_instanceScales.push_back(normalizedSize);

	++m_nInstances;
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
