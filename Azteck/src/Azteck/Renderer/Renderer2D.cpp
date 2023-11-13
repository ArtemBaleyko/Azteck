#include "azpch.h"
#include "Renderer2D.h"
#include "VertexArray.h"
#include "Shader.h"
#include "UniformBuffer.h"

#include "RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "MSDFData.h"

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

	struct CircleVertex
	{
		glm::vec3 worldPosition;
		glm::vec3 localPosition;
		glm::vec4 color;
		float thickness;
		float fade;

		// Editor only
		int entityID = -1;
	};

	struct LineVertex
	{
		glm::vec3 position;
		glm::vec4 color;

		// Editor only
		int entityID = -1;
	};

	struct TextVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoord;

		// TODO: bg color for outline/bg

		// Editor-only
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
		Ref<Shader> quadShader;
		uint32_t quadIndexCount = 0;
		QuadVertex* quadVertexBufferBase = nullptr;
		QuadVertex* quadVertexBufferPtr = nullptr;

		Ref<VertexArray> circleVertexArray;
		Ref<VertexBuffer> circleVertexBuffer;
		Ref<Shader> circleShader;
		uint32_t circleIndexCount = 0;
		CircleVertex* circleVertexBufferBase = nullptr;
		CircleVertex* circleVertexBufferPtr = nullptr;

		Ref<VertexArray> lineVertexArray;
		Ref<VertexBuffer> lineVertexBuffer;
		Ref<Shader> lineShader;
		uint32_t lineVertexCount = 0;
		LineVertex* lineVertexBufferBase = nullptr;
		LineVertex* lineVertexBufferPtr = nullptr;
		float lineWidth = 1.0f;

		Ref<VertexArray> textVertexArray;
		Ref<VertexBuffer> textVertexBuffer;
		Ref<Shader> textShader;
		uint32_t textIndexCount = 0;
		TextVertex* textVertexBufferBase = nullptr;
		TextVertex* textVertexBufferPtr = nullptr;

		Ref<Texture2D> whiteTexture;

		std::array<Ref<Texture2D>, maxTextureSlots> textureSlots;
		uint32_t textureSlotIndex = 1;

		Ref<Texture2D> fontAtlasTexture;

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

		initQuads();
		initCircles();
		initLines();
		initText();

		_data.whiteTexture = Texture2D::create(TextureSpecification{});
		uint32_t whiteTextureData = 0xffffffff;
		_data.whiteTexture->setData(&whiteTextureData, sizeof(whiteTextureData));

		int32_t samplers[_data.maxTextureSlots];
		for (int32_t i = 0; i < _data.maxTextureSlots; i++)
			samplers[i] = i;

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

		_data.quadShader->bind();
		_data.quadShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

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
		if (_data.quadIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)_data.quadVertexBufferPtr - (uint8_t*)_data.quadVertexBufferBase);
			_data.quadVertexBuffer->setData(_data.quadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
				_data.textureSlots[i]->bind(i);

			_data.quadShader->bind();
			RenderCommand::drawIndexed(_data.quadVertexArray, _data.quadIndexCount);

			_data.stats.drawCalls++;
		}

		if (_data.circleIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)_data.circleVertexBufferPtr - (uint8_t*)_data.circleVertexBufferBase);
			_data.circleVertexBuffer->setData(_data.circleVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
				_data.textureSlots[i]->bind(i);

			_data.circleShader->bind();
			RenderCommand::drawIndexed(_data.circleVertexArray, _data.circleIndexCount);

			_data.stats.drawCalls++;
		}

		if (_data.lineVertexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)_data.lineVertexBufferPtr - (uint8_t*)_data.lineVertexBufferBase);
			_data.lineVertexBuffer->setData(_data.lineVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < _data.textureSlotIndex; i++)
				_data.textureSlots[i]->bind(i);

			_data.lineShader->bind();
			RenderCommand::setLineWidth(_data.lineWidth);
			RenderCommand::drawLines(_data.lineVertexArray, _data.lineVertexCount);

			_data.stats.drawCalls++;
		}

		if (_data.textIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)_data.textVertexBufferPtr - (uint8_t*)_data.textVertexBufferBase);
			_data.textVertexBuffer->setData(_data.textVertexBufferBase, dataSize);

			_data.fontAtlasTexture->bind(0);

			_data.textShader->bind();
			RenderCommand::drawIndexed(_data.textVertexArray, _data.textIndexCount);
			_data.stats.drawCalls++;
		}
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

	void Renderer2D::drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID)
	{
		AZ_PROFILE_FUNCTION();

		constexpr size_t circleVertexCount = 4;

		for (size_t i = 0; i < circleVertexCount; i++)
		{
			_data.circleVertexBufferPtr->worldPosition = transform * _data.quadVertexPositions[i];
			_data.circleVertexBufferPtr->localPosition = _data.quadVertexPositions[i] * 2.0f;
			_data.circleVertexBufferPtr->color = color;
			_data.circleVertexBufferPtr->thickness = thickness;
			_data.circleVertexBufferPtr->fade = fade;
			_data.circleVertexBufferPtr->entityID = entityID;
			_data.circleVertexBufferPtr++;
		}

		_data.circleIndexCount += 6;

		_data.stats.quadCount++;
	}

	void Renderer2D::drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, int entityID)
	{
		_data.lineVertexBufferPtr->position = start;
		_data.lineVertexBufferPtr->color = color;
		_data.lineVertexBufferPtr->entityID = entityID;
		_data.lineVertexBufferPtr++;

		_data.lineVertexBufferPtr->position = end;
		_data.lineVertexBufferPtr->color = color;
		_data.lineVertexBufferPtr->entityID = entityID;
		_data.lineVertexBufferPtr++;

		_data.lineVertexCount += 2;
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID)
	{
		glm::vec3 p0 = glm::vec3(position.x - size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + size.x * 0.5f, position.y - size.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + size.x * 0.5f, position.y + size.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - size.x * 0.5f, position.y + size.y * 0.5f, position.z);

		drawLine(p0, p1, color);
		drawLine(p1, p2, color);
		drawLine(p2, p3, color);
		drawLine(p3, p0, color);
	}

	void Renderer2D::drawRect(const glm::mat4& transform, const glm::vec4& color, int entityID)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * _data.quadVertexPositions[i];

		drawLine(lineVertices[0], lineVertices[1], color);
		drawLine(lineVertices[1], lineVertices[2], color);
		drawLine(lineVertices[2], lineVertices[3], color);
		drawLine(lineVertices[3], lineVertices[0], color);
	}

	void Renderer2D::drawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID)
	{
		if (src.texture)
			drawQuad(transform, src.texture, src.color, src.tilingFactor, entityID);
		else
			drawQuad(transform, src.color, entityID);
	}

	void Renderer2D::drawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID)
	{
		const auto& fontGeometry = font->getMSDFData()->fontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		Ref<Texture2D> fontAtlas = font->getAtlasTexture();

		_data.fontAtlasTexture = fontAtlas;

		double x = 0.0;
		double y = 0.0;

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		const double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);

		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.lineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.kerning;
				continue;
			}

			if (character == '\t')
			{
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);

			if (!glyph)
				glyph = fontGeometry.getGlyph('?');

			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->getWidth();
			float texelHeight = 1.0f / fontAtlas->getHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			_data.textVertexBufferPtr->position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			_data.textVertexBufferPtr->color = textParams.color;
			_data.textVertexBufferPtr->texCoord = texCoordMin;
			_data.textVertexBufferPtr->entityID = entityID; // TODO
			_data.textVertexBufferPtr++;

			_data.textVertexBufferPtr->position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			_data.textVertexBufferPtr->color = textParams.color;
			_data.textVertexBufferPtr->texCoord = { texCoordMin.x, texCoordMax.y };
			_data.textVertexBufferPtr->entityID = entityID; // TODO
			_data.textVertexBufferPtr++;

			_data.textVertexBufferPtr->position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			_data.textVertexBufferPtr->color = textParams.color;
			_data.textVertexBufferPtr->texCoord = texCoordMax;
			_data.textVertexBufferPtr->entityID = entityID; // TODO
			_data.textVertexBufferPtr++;

			_data.textVertexBufferPtr->position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			_data.textVertexBufferPtr->color = textParams.color;
			_data.textVertexBufferPtr->texCoord = { texCoordMax.x, texCoordMin.y };
			_data.textVertexBufferPtr->entityID = entityID; // TODO
			_data.textVertexBufferPtr++;

			_data.textIndexCount += 6;
			_data.stats.quadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.kerning;
			}
		}
	}

	void Renderer2D::drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID)
	{
		drawString(string, component.fontAsset, transform, { component.color, component.kerning, component.lineSpacing }, entityID);
	}

	float Renderer2D::getLineWidth()
	{
		return _data.lineWidth;
	}

	void Renderer2D::setLineWidth(float width)
	{
		_data.lineWidth = width;
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
		_data.quadVertexBufferPtr = _data.quadVertexBufferBase;

		_data.circleIndexCount = 0;
		_data.circleVertexBufferPtr = _data.circleVertexBufferBase;

		_data.lineVertexCount = 0;
		_data.lineVertexBufferPtr = _data.lineVertexBufferBase;

		_data.textIndexCount = 0;
		_data.textVertexBufferPtr = _data.textVertexBufferBase;

		_data.textureSlotIndex = 1;
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

	void Renderer2D::initQuads()
	{
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

		_data.quadShader = Shader::create("assets/shaders/Renderer2D_Quad.glsl");
	}

	void Renderer2D::initCircles()
	{
		_data.circleVertexArray = VertexArray::create();
		_data.circleVertexBuffer = VertexBuffer::create(_data.maxVertices * sizeof(CircleVertex));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_WorldPosition"},
			{ShaderDataType::Float3, "a_LocalPosition"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float, "a_Thickness"},
			{ShaderDataType::Float, "a_Fade"},
			{ShaderDataType::Int, "a_EntityID"}
		};

		_data.circleVertexBuffer->setLayout(layout);
		_data.circleVertexArray->addVertexBuffer(_data.circleVertexBuffer);
		_data.circleVertexBufferBase = new CircleVertex[_data.maxVertices];

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
		_data.circleVertexArray->setIndexBuffer(indexBuffer);
		delete[] indices;

		_data.circleShader = Shader::create("assets/shaders/Renderer2D_Circle.glsl");
	}

	void Renderer2D::initLines()
	{
		_data.lineVertexArray = VertexArray::create();
		_data.lineVertexBuffer = VertexBuffer::create(_data.maxVertices * sizeof(LineVertex));

		BufferLayout layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Int, "a_EntityID"}
		};

		_data.lineVertexBuffer->setLayout(layout);
		_data.lineVertexArray->addVertexBuffer(_data.lineVertexBuffer);
		_data.lineVertexBufferBase = new LineVertex[_data.maxVertices];

		_data.lineShader = Shader::create("assets/shaders/Renderer2D_Line.glsl");
	}

	void Renderer2D::initText()
	{
		_data.textVertexArray = VertexArray::create();

		_data.textVertexBuffer = VertexBuffer::create(_data.maxVertices * sizeof(TextVertex));
		_data.textVertexBuffer->setLayout({
			{ ShaderDataType::Float3, "a_Position"     },
			{ ShaderDataType::Float4, "a_Color"        },
			{ ShaderDataType::Float2, "a_TexCoord"     },
			{ ShaderDataType::Int,    "a_EntityID"     }
			});

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
		_data.textVertexArray->addVertexBuffer(_data.textVertexBuffer);
		_data.textVertexArray->setIndexBuffer(indexBuffer);
		_data.textVertexBufferBase = new TextVertex[_data.maxVertices];

		_data.textShader = Shader::create("assets/shaders/Renderer2D_Text.glsl");
	}
}
