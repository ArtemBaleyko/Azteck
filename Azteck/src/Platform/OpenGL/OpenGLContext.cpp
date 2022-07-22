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

	#ifdef AZ_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		AZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires at least OpenGL version 4.5!");
	#endif
	}

	void OpenGLContext::swapBuffers()
	{
		AZ_PROFILE_FUNCTION();

		glfwSwapBuffers(_windowHandle);
	}

}