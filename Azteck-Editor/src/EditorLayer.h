#pragma once

#include <Azteck.h>

namespace Azteck
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		void onAttach() override;
		void onDetach() override;

		void onUpdate(Timestep timestep) override;
		void onEvent(Event& e) override;
		void onImGuiRender() override;

	private:
		OrthographicCameraController _cameraController;

		// Temporary
		Ref<Texture2D> _texture;
		Ref<FrameBuffer> _frameBuffer;
		glm::vec4 _squareColor;
	};
}
