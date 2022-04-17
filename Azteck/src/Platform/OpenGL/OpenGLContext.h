#pragma once

#include "Azteck/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Azteck
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		void init() override;
		void swapBuffers() override;

	private:
		GLFWwindow* _windowHandle;
	};
}