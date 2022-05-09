#include "azpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"

#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Azteck
{
	struct Renderer2DStorage
	{
		Ref<VertexArray> vertexArray;
		Ref<Shader> textureShader;
		Ref<Texture2D> whiteTexture;
	};

	static Renderer2DStorage* _data;

	void Renderer2D::init()
	{
		_data = new Renderer2DStorage;
		_data->vertexArray = VertexArray::create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		Ref<VertexBuffer> vertexBuffer;
		vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->setLayout(layout);
		_data->vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> indexBuffer;
		indexBuffer = IndexBuffer::create(indices, 6);

		_data->vertexArray->setIndexBuffer(indexBuffer);

		_data->whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		_data->whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		_data->textureShader = Shader::create("assets/shaders/Texture.glsl");
		_data->textureShader->bind();
		_data->textureShader->setInt("u_Texture", 0);
	}

	void Renderer2D::shutdown()
	{
		delete _data;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		_data->textureShader->bind();
		_data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene()
	{

	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(position, size, _data->whiteTexture, color);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, texture);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture)
	{
		drawQuad(position, size, texture, glm::vec4(1.0f));
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		texture->bind();

		_data->textureShader->setMat4("u_Transform", transform);
		_data->textureShader->setFloat4("u_Color", color);

		_data->vertexArray->bind();
		RenderCommand::drawIndexed(_data->vertexArray);
	}
}