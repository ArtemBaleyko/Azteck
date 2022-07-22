#include "azpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Azteck
{
	Scope<RendererAPI> RenderCommand::_rendererAPI = RendererAPI::create();
}