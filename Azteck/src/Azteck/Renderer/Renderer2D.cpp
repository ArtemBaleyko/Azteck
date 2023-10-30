#include "azpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "UniformBuffer.h"

#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Azteck
{
	struct QuadVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;
		float texIndex;
		float tilingFactor;

		// Editor only
		int entityID = -1;
	};

	struct Renderer2DData
	{
		static const uint32_t maxQuads = 10000;
		static const uint32_t maxVertices = maxQuads * 4;
		static const uint32_t maxIndices = maxQuads * 6;
		static const uint32_t maxTextureSlots = 32;

		Ref<VertexArray> quadVertexArray;
		Ref<VertexBuffer> quadVertexBuffer;
		Ref<Shader> quadTextureShader;
		Ref<Texture2D> whiteTexture;

		uint32_t quadIndexCount = 0;

		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;

		const glm::vec4 quadVertexPositions[4] = {
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f }
		};

		const glm::vec2 quadTexCoords[4] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		Renderer2D::Statistics stats;

		struct CameraData
		{
			glm::mat4 viewProjection;
		};

		CameraData cameraBuffer;
		Ref<UniformBuffer> cameraUniformBuffer;
	};

	static Renderer2DData _data;

	void Renderer2D::init()
	{
		AZ_PROFILE_FUNCTION();

		_data.quadVertexArray = VertexArray::create();

		_data.quadVertexBuffer = VertexBuffer::create(_data.maxVertices * sizeof(QuadVertex));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"},
			{ShaderDataType::Int, "a_EntityID"}
		};

		_data.quadVertexBuffer->setLayout(layout);
		_data.quadVertexArray->addVertexBuffer(_data.quadVertexBuffer);

		_data.quadVertexBufferBase = new QuadVertex[_data.maxVertices];

		uint32_t* indices = new uint32_t[_data.maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < _data.maxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		Ref<IndexBuffer> indexBuffer = IndexBuffer::create(indices, _data.maxIndices);
		_data.quadVertexArray->setIndexBuffer(indexBuffer);

		delete[] indices;

		_data.whiteTexture = Texture2D::create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		_data.whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		int32_t samplers[_data.maxTextureSlots];
		for (int32_t i = 0; i < _data.maxTextureSlots; i++)
			samplers[i] = i;

		_data.quadTextureShader = Shader::create("assets/shaders/Texture.glsl");
		_data.textureSlots[0] = _data.whiteTexture;

		_data.cameraUniformBuffer = UniformBuffer::create(sizeof(Renderer2DData::CameraData), 0);
	}

	void Renderer2D::shutdown()
	{
		AZ_PROFILE_FUNCTION();

		delete[] _data.quadVertexBufferBase;
	}

	// TODO: Remove
	void Renderer2D::beginScene(const OrthographicCamera& camera)
	{
		AZ_PROFILE_FUNCTION();

		_data.quadTextureShader->bind();
		_data.quadTextureShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		startBatch();
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform)
	{
		AZ_PROFILE_FUNCTION();

		_data.cameraBuffer.viewProjection = camera.getProjection() * glm::inverse(transform);
		_data.cameraUniformBuffer->setData(&_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		startBatch();
	}

	void Renderer2D::beginScene(const EditorCamera& camera)
	{
		AZ_PROFILE_FUNCTION();

		_data.cameraBuffer.viewProjection = camera.getViewProjection();
		_data.cameraUniformBuffer->setData(&_data.cameraBuffer, sizeof(Renderer2DData::CameraData));

		startBatch();
	}

	void Renderer2D::endScene()
	{
		AZ_PROFILE_FUNCTION();

		flush();
	}

	void Renderer2D::flush()
	{
		if (_data.quadIndexCount == 0)
			return;

		uint32_t dataSize = (uint32_t)((uint8_t*)_data.quadVertexBufferPtr - (uint8_t*)_data.quadVertexBufferBase);
		_data.quadVertexBuffer->setData(_data.quadVertexBufferBase, dataSize);

		for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
			_data.textureSlots[i]->bind(i);

		_data.quadTextureShader->bind();
		RenderCommand::drawIndexed(_data.quadVertexArray, _data.quadIndexCount);

		_data.stats.drawCalls++;
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		drawQuad(position, size, _data.whiteTexture, color, 1.0f);
	}

	void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawQuad(position, size, texture, tintColor, tilingFactor);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		drawQuad(transform, _data.whiteTexture, color, 1.0f, entityID);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor, int entityID)
	{
		drawQuad(transform, texture, tintColor, tilingFactor, entityID);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		drawRotatedQuad(position, size, rotation, _data.whiteTexture, color, 1.0f);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		drawRotatedQuad(position, size, rotation, texture, tintColor, tilingFactor);
	}

	void Renderer2D::drawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID)
	{
		if (src.texture)
			drawQuad(transform, src.texture, src.color, src.tilingFactor, entityID);
		else
			drawQuad(transform, src.color, entityID);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor, int entityID)
	{
		constexpr size_t quadVertexCount = 4;

		if (_data.quadIndexCount >= _data.maxIndices)
			nextBatch();

		float textureIndex = -1.0f;

		for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
		{
			if (*_data.textureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			if (_data.textureSlotIndex >= Renderer2DData::maxTextureSlots)
				nextBatch();

			textureIndex = (float)_data.textureSlotIndex;
			_data.textureSlots[_data.textureSlotIndex] = texture;

			_data.textureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++)
		{
			_data.quadVertexBufferPtr->position = transform * _data.quadVertexPositions[i];
			_data.quadVertexBufferPtr->color = color;
			_data.quadVertexBufferPtr->texCoord = _data.quadTexCoords[i];
			_data.quadVertexBufferPtr->texIndex = textureIndex;
			_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			_data.quadVertexBufferPtr->entityID = entityID;
			_data.quadVertexBufferPtr++;
		}

		_data.quadIndexCount += 6;

		_data.stats.quadCount++;
	}

	void Renderer2D::drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		drawQuad(transform, texture, color, tilingFactor);
	}

	void Renderer2D::drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor)
	{
		AZ_PROFILE_FUNCTION();

		if (_data.quadIndexCount >= _data.maxIndices)
			nextBatch();

		float textureIndex = -1.0f;

		for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
		{
			if (*_data.textureSlots[i] == *texture)
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == -1.0f)
		{
			if (_data.textureSlotIndex >= Renderer2DData::maxTextureSlots)
				nextBatch();

			textureIndex = (float)_data.textureSlotIndex;
			_data.textureSlots[_data.textureSlotIndex] = texture;

			_data.textureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		transform = glm::rotate(transform, glm::radians(rotation), { 0.0f, 0.0f, 1.0f });
		transform = glm::scale(transform, { size.x, size.y, 1.0f });

		for (size_t i = 0; i < 4; i++)
		{
			_data.quadVertexBufferPtr->position = transform * _data.quadVertexPositions[i];
			_data.quadVertexBufferPtr->color = color;
			_data.quadVertexBufferPtr->texCoord = _data.quadTexCoords[i];
			_data.quadVertexBufferPtr->texIndex = textureIndex;
			_data.quadVertexBufferPtr->tilingFactor = tilingFactor;
			_data.quadVertexBufferPtr++;
		}

		_data.quadIndexCount += 6;

		_data.stats.quadCount++;
	}

	void Renderer2D::startBatch()
	{
		_data.quadIndexCount = 0;
		_data.textureSlotIndex = 1;
		_data.quadVertexBufferPtr = _data.quadVertexBufferBase;
	}

	void Renderer2D::nextBatch()
	{
		flush();
		startBatch();
	}

	void Renderer2D::resetStats()
	{
		_data.stats.drawCalls = 0;
		_data.stats.quadCount = 0;
	}

	Renderer2D::Statistics Renderer2D::getStats()
	{
		return _data.stats;
	}
}
