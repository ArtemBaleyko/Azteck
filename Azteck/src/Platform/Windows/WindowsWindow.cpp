#include "azpch.h"
#include "WindowsWindow.h"

#include "Azteck/Renderer/Renderer.h"
#include "Azteck/Events/ApplicationEvent.h"
#include "Azteck/Events/KeyEvent.h"
#include "Azteck/Events/MouseEvent.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace Azteck
{
	static bool glfwInitialized = false;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		AZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
		return createScope<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		AZ_PROFILE_FUNCTION();

		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		AZ_PROFILE_FUNCTION();

		shutdown();
	}

	void WindowsWindow::onUpdate()
	{
		AZ_PROFILE_FUNCTION();

		glfwPollEvents();
		_context->swapBuffers();
	}

	void WindowsWindow::setEventCallback(const EventCallbackFn& callback)
	{
		_data.eventCallback = callback;
	}

	void WindowsWindow::setVSync(bool enabled)
	{
		AZ_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		_data.vSync = enabled;
	}

	bool WindowsWindow::isVSync() const
	{
		return _data.vSync;
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		AZ_PROFILE_FUNCTION();

		_data.title = props.title;
		_data.width = props.width;
		_data.height = props.height;

		AZ_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!glfwInitialized)
		{
			int success = glfwInit();
			AZ_CORE_ASSERT(success, "Could not initialize GLFW!");

			glfwInitialized = true;
		}

	#if defined(AZ_DEBUG)
			if (Renderer::getAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	#endif

		_window = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), _data.title.c_str(), nullptr, nullptr);
		_context = GraphicsContext::create(_window);
		_context->init();

		glfwSetWindowUserPointer(_window, &_data);
		setVSync(true);

		// Callbacks
		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.width = width;
				data.height = height;

				WindowResizedEvent event(width, height);
				data.eventCallback(event);
			});

		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.eventCallback(event);
			});

		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.eventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.eventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.eventCallback(event);
						break;
					}
					default:
						break;
				}

			});

		glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int keyCode) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keyCode);
				data.eventCallback(event);
			});

		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.eventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.eventCallback(event);
					break;
				}
				default:
					break;
				}
			});

		glfwSetScrollCallback(_window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.eventCallback(event);
			});

		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPox, double yPos) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPox, (float)yPos);
				data.eventCallback(event);
			});
	}

	void WindowsWindow::shutdown()
	{
		AZ_PROFILE_FUNCTION();

		glfwDestroyWindow(_window);
	} 
}
