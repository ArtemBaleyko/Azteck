#include <azpch.h>
#include "Window.h"

#ifdef AZ_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Azteck
{
	Scope<Window> Window::create(const WindowProps& props)
	{
#ifdef AZ_PLATFORM_WINDOWS
		return createScope<WindowsWindow>(props);
#else
		AZ_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}