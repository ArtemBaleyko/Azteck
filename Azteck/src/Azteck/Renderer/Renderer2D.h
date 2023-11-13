#pragma once

#include "OrthographicCamera.h"
#include "Texture.h"
#include "Camera.h"
#include "EditorCamera.h"
#include "Azteck/Scene/Components.h"
#include "Font.h"

namespace Azteck
{
	class Renderer2D
	{
	public:
		struct TextParams
		{
			glm::vec4 color{ 1.0f };
			float kerning = 0.0f;
			float lineSpacing = 0.0f;
		};

	public:
		static void init();
		static void shutdown();

		static void beginScene(const OrthographicCamera& camera); // TODO: remove
		static void beginScene(const Camera& camera, const glm::mat4& transform);
		static void beginScene(const EditorCamera& camera);
		static void endScene();
		static void flush();

		// Primitives
		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);

		static void drawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void drawQuad(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f), int entityID = -1);

		static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);

		static void drawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

		static void drawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness, float fade, int entityID = -1);

		static void drawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, int entityID = -1);

		static void drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color, int entityID = -1);
		static void drawRect(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);

		static void drawSprite(const glm::mat4& transform, const SpriteRendererComponent& src, int entityID = -1);

		static void drawString(const std::string& string, Ref<Font> font, const glm::mat4& transform, const TextParams& textParams, int entityID = -1);
		static void drawString(const std::string& string, const glm::mat4& transform, const TextComponent& component, int entityID = -1);

		static float getLineWidth();
		static void setLineWidth(float width);

		// Stats
		struct Statistics
		{
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;

			inline uint32_t getTotalVertexCount() const { return quadCount * 4; }
			inline uint32_t getTotalIndexCount() const { return quadCount * 6; }
		};

		static void resetStats();
		static Statistics getStats();

	private:
		static void initQuads();
		static void initCircles();
		static void initLines();
		static void initText();

		static void drawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor, int entityID = -1);

		static void drawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor);
		static void drawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const Ref<Texture2D>& texture, const glm::vec4& color, float tilingFactor);

		static void startBatch();
		static void nextBatch();
	};
}
