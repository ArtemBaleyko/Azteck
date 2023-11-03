#include "azpch.h"

#include "Azteck/Core/Application.h"
#include "Azteck/Core/Log.h"
#include "Azteck/Core/Input.h"
#include "Azteck/Renderer/Renderer.h"

#include "Azteck/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

#include <glfw/glfw3.h>

namespace Azteck
{
	Application* Application::_instance = nullptr;

	Application::Application(const ApplicationSpecification& spec)
		: _isRunning(true)
		, _isMinimised(false)
		, _lastFrameTime(0.0f)
		, _spec(spec)
	{
		AZ_PROFILE_FUNCTION();

		AZ_CORE_ASSERT(!_instance, "Application already exists");
		_instance = this;

		if (!_spec.workingDirectory.empty())
			std::filesystem::current_path(_spec.workingDirectory);

		_window = Window::create(WindowProps(_spec.name));
		_window->setEventCallback(AZ_BIND_EVENT_FN(Application::onEvent));

		Renderer::init();
		ScriptEngine::init();

		_imGuiLayer = new ImGuiLayer;
		pushOverlay(_imGuiLayer);
	}

	Application::~Application()
	{
		ScriptEngine::shutdown();
		Renderer::shutdown();
	}

	void Application::close()
	{
		_isRunning = false;
	}

	void Application::onEvent(Event& e)
	{
		AZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowCloseEvent>(AZ_BIND_EVENT_FN(Application::onWindowClose));
		dispatcher.dispatch<WindowResizedEvent>(AZ_BIND_EVENT_FN(Application::onWindowResized));

		for (auto it = _layerStack.rbegin(); it != _layerStack.rend(); ++it)
		{
			if (e.isHandled())
				break;

			(*it)->onEvent(e);
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		if (layer == nullptr)
			return;

		_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		if (layer == nullptr)
			return;

		_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		close();
		return true;
	}

	bool Application::onWindowResized(WindowResizedEvent& e)
	{
		AZ_PROFILE_FUNCTION();

		uint32_t width = e.getWidth();
		uint32_t height = e.getHeight();

		_isMinimised = width == 0 || height == 0;

		if (_isMinimised)
			return false;

		Renderer::onWindowResized(width, height);
		return false;
	}

	void Application::run()
	{
		AZ_PROFILE_FUNCTION();

		while (_isRunning)
		{
			AZ_PROFILE_SCOPE("Run loop");

			float time = (float)glfwGetTime();
			Timestep timestep = time - _lastFrameTime;
			_lastFrameTime = time;

			if (!_isMinimised)
			{
				{
					AZ_PROFILE_SCOPE("Layer updates");

					for (Layer* layer : _layerStack)
						layer->onUpdate(timestep);
				}

				_imGuiLayer->begin();
				{
					AZ_PROFILE_SCOPE("Layer imGui rendering");
					for (Layer* layer : _layerStack)
						layer->onImGuiRender();
				}
				_imGuiLayer->end();
			}

			_window->onUpdate();
		}
	}
}
