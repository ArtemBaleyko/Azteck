#include "azpch.h"
#include "WindowsWindow.h"

#include "Azteck/Events/ApplicationEvent.h"
#include "Azteck/Events/KeyEvent.h"
#include "Azteck/Events/MouseEvent.h"

#include <glad/glad.h>

namespace Azteck
{
	static bool glfwInitialized = false;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		AZ_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}

	void WindowsWindow::onUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(_window);
	}

	void WindowsWindow::setEventCallback(const EventCallbackFn& callback)
	{
		_data.eventCallback = callback;
	}

	void WindowsWindow::setVSync(bool enabled)
	{
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

		_window = glfwCreateWindow(static_cast<int>(props.width), static_cast<int>(props.height), _data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AZ_CORE_ASSERT(status, "Failed to initialize GLAD!");

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

				MouseScrolledEvent event(xOffset, yOffset);
				data.eventCallback(event);
			});

		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xPox, double yPos) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event(xPox, yPos);
				data.eventCallback(event);
			});
	}

	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(_window);
	} 
}
