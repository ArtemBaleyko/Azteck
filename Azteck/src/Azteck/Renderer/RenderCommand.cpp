#include "azpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Azteck
{
	RendererAPI* RenderCommand::_rendererAPI = new OpenGLRendererAPI;
}