#include "azpch.h"
#include "Azteck/Core/Input.h"
#include "Azteck/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Azteck
{
	bool Input::isKeyPressed(KeyCode keyCode)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("Input::isKeyPressedImpl - Unable to get a native window");
			return false;
		}

		int state = glfwGetKey(window, static_cast<int32_t>(keyCode));
		return state == GLFW_PRESS;
	}

	bool Input::isMouseButtonPressed(MouseCode button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("Input::isMouseButtonPressedImpl - Unable to get a native window");
			return false;
		}

		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::getMousePosition()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());

		if (window == nullptr)
		{
			AZ_CORE_ERROR("Input::getMouseXImpl - Unable to get a native window");
			return {0.0f, 0.0f};
		}

		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);

		return { static_cast<float>(xPos), static_cast<float>(yPos) };
	}

	float Input::getMouseX()
	{
		return getMousePosition().x;
	}

	float Input::getMouseY()
	{
		return getMousePosition().y;
	}
}
