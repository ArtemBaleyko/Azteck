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
		AZ_PROFILE_FUNCTION();

		glfwMakeContextCurrent(_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AZ_CORE_ASSERT(status, "Failed to initialize GLAD!");

		AZ_CORE_INFO("OpenGL Info:");
		AZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		AZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		AZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		AZ_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5), "Azteck requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::swapBuffers()
	{
		AZ_PROFILE_FUNCTION();

		glfwSwapBuffers(_windowHandle);
	}

}
