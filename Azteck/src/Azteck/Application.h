#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Azteck/LayerStack.h"
#include "Window.h"

#include "ImGui/ImGuiLayer.h"

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

	private:
		std::unique_ptr<Window> _window;
		ImGuiLayer* _imGuiLayer;
		bool _isRunning;
		LayerStack _layerStack;

		static Application* _instance;
	};

	// To be defined in client
	Application* createApplication();
}

