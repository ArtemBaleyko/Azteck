#include <azpch.h>
#include "Input.h"

#ifdef AZ_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
#endif

namespace Azteck
{
	Scope<Input> Input::_instance = Input::create();

	Scope<Input> Input::create()
	{
#ifdef AZ_PLATFORM_WINDOWS
		return createScope<WindowsInput>();
#else
		AZ_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
