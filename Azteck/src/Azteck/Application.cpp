#include "azpch.h"
#include "Application.h"
#include "Azteck/Log.h"
#include "Input.h"
#include "Azteck/Renderer/Renderer.h"

#include <glm/glm.hpp>

#include <glfw/glfw3.h>

namespace Azteck
{
	Application* Application::_instance = nullptr;

	Application::Application()
		: _isRunning(true)
		, _lastFrameTime(0.0f)
	{
		AZ_CORE_ASSERT(!_instance, "Application already exists");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::Create());
		_window->setEventCallback(AZ_BIND_EVENT_FN(Application::onEvent));

		Renderer::init();

		_imGuiLayer = new ImGuiLayer;
		pushOverlay(_imGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowCloseEvent>(AZ_BIND_EVENT_FN(Application::onWindowClose));

		for (auto it = _layerStack.end(); it != _layerStack.begin();)
		{
			(*--it)->onEvent(e);
			if (e.isHandled())
				break;
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		_isRunning = false;
		return true;
	}

	void Application::run()
	{
		while (_isRunning)
		{
			float time = (float)glfwGetTime();
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			for (Layer* layer : _layerStack)
				layer->onUpdate(timestep);

			_imGuiLayer->begin();

			for (Layer* layer : _layerStack)
				layer->onImGuiRender();

			_imGuiLayer->end();
			_window->onUpdate();
		}
	}
}
