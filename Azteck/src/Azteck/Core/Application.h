#pragma once

#include "Azteck/Core/Core.h"
#include "Azteck/Events/Event.h"
#include "Azteck/Events/ApplicationEvent.h"
#include "Azteck/Core/LayerStack.h"
#include "Azteck/Core/Window.h"
#include "Azteck/ImGui/ImGuiLayer.h"
#include "Azteck/Renderer/Shader.h"
#include "Azteck/Renderer/Buffer.h"
#include "Azteck/Renderer/VertexArray.h"
#include "Azteck/Renderer/OrthographicCamera.h"
#include "Azteck/Core/Timestep.h"

int main(int argc, char** argv);

namespace Azteck
{
	struct ApplicationCommandLineArgs
	{
		int count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			AZ_CORE_ASSERT(index < count, "Invalid index");
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string name = "Azteck App";
		std::string workingDirectory;

		ApplicationCommandLineArgs args;
	};

	class  Application
	{
	public:
		Application(const ApplicationSpecification& spec);
		virtual ~Application();

		void close();

		void onEvent(Event& e);

		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);

		inline ImGuiLayer* getImGuiLayer() const { return _imGuiLayer; }
		inline Window& getWindow() const { return *_window; }

		inline static Application& getInstance() { return *_instance; }
		inline const ApplicationSpecification& getSpecification() const { return _spec; }

	private:
		void run();

		bool onWindowClose(WindowCloseEvent& e);
		bool onWindowResized(WindowResizedEvent& e);

	private:
		ApplicationSpecification _spec;
		Scope<Window> _window;

		ImGuiLayer* _imGuiLayer;
		LayerStack _layerStack;

		bool _isMinimised;
		bool _isRunning;
		float _lastFrameTime;

	private:
		static Application* _instance;

		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* createApplication(ApplicationCommandLineArgs args);
}

