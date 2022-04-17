#include "azpch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include <glad/glad.h>

namespace Azteck
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: _windowHandle(windowHandle)
	{
		AZ_CORE_ASSERT(windowHandle, "Window handle is null");
	}

	void OpenGLContext::init()
	{
		glfwMakeContextCurrent(_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AZ_CORE_ASSERT(status, "Failed to initialize GLAD!");

		AZ_CORE_INFO("OpenGL Info:");
		AZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		AZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		AZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::swapBuffers()
	{
		glfwSwapBuffers(_windowHandle);
	}

}