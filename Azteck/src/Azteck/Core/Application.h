#pragma once

#include "Core.h"
#include "Azteck/Events/Event.h"
#include "Azteck/Events/ApplicationEvent.h"
#include "Azteck/Core/LayerStack.h"
#include "Window.h"

#include "Azteck/ImGui/ImGuiLayer.h"

#include "Azteck/Renderer/Shader.h"
#include "Azteck/Renderer/Buffer.h"
#include "Azteck/Renderer/VertexArray.h"
#include "Azteck/Renderer/OrthographicCamera.h"

#include "Azteck/Core/Timestep.h"

namespace Azteck
{
	class AZTECK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline Window& getWindow() const { return *_window; }

		inline static Application& getInstance() { return *_instance; }

	private:
		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResized(WindowResizedEvent& e);

	private:
		std::unique_ptr<Window> _window;

		ImGuiLayer* _imGuiLayer;
		LayerStack _layerStack;

		bool _isMinimised;
		bool _isRunning;
		float _lastFrameTime;
	private:
		static Application* _instance;
	};

	// To be defined in client
	Application* createApplication();
}

