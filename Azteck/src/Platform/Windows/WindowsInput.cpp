#include "azpch.h"
#include "WindowsInput.h"
#include "Azteck/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Azteck
{
	Input* Input::_instance = new WindowsInput();

	bool WindowsInput::isKeyPressedImpl(int keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("WindowsInput::isKeyPressedImpl - Unable to get a native window");
			return false;
		}

		int state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS;
	}

	bool WindowsInput::isMouseButtonPressedImpl(int button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("WindowsInput::isMouseButtonPressedImpl - Unable to get a native window");
			return false;
		}

		int state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::getMousePossitionImpl()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("WindowsInput::getMouseXImpl - Unable to get a native window");
			return {0.0f, 0.0f};
		}

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { static_cast<float>(xPos), static_cast<float>(yPos) };
	}

	float WindowsInput::getMouseXImpl()
	{
		const auto [xPos, yPos] = getMousePossitionImpl();

		return xPos;
	}

	float WindowsInput::getMouseYImpl()
	{
		const auto [xPos, yPos] = getMousePossitionImpl();

		return yPos;
	}
}