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
		AZ_PROFILE_FUNCTION();

		_data = new Renderer2DStorage;
		_data->vertexArray = VertexArray::create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		Ref<VertexBuffer> vertexBuffer = VertexBuffer::create(vertices, sizeof(vertices));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->setLayout(layout);
		_data->vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, 6);

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
		AZ_PROFILE_FUNCTION();

		delete _data;
	}

	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		AZ_PROFILE_FUNCTION();

		_data->textureShader->bind();
		_data->textureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene()
	{
		AZ_PROFILE_FUNCTION();
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(position, size, _data->whiteTexture, color, 1.0f);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad(position, size, texture, tintColor, tilingFactor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad(position, size, rotation, _data->whiteTexture, color, 1.0f);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawRotatedQuad(position, size, rotation, texture, tintColor, tilingFactor);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		AZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		texture->bind();

		_data->textureShader->setMat4("u_Transform", transform);
		_data->textureShader->setFloat4("u_Color", color);
		_data->textureShader->setFloat("u_TilingFactor", tilingFactor);

		_data->vertexArray->bind();
		RenderCommand::drawIndexed(_data->vertexArray);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		AZ_PROFILE_FUNCTION();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, rotation, { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		texture->bind();

		_data->textureShader->setMat4("u_Transform", transform);
		_data->textureShader->setFloat4("u_Color", color);
		_data->textureShader->setFloat("u_TilingFactor", tilingFactor);

		_data->vertexArray->bind();
		RenderCommand::drawIndexed(_data->vertexArray);
	}
}