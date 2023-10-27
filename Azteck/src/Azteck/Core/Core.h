#pragma once

#include <memory>

#include "Azteck/Core/PlatformDetection.h"


//// DLL support
//#ifdef AZ_PLATFORM_WINDOWS
//	#if AZ_DYNAMIC_LINK
//		#ifdef AZ_BUILD_DLL
//			#define  __declspec(dllexport)
//		#else
//			#define  __declspec(dllimport)
//		#endif
//	#else
//		#define 
//	#endif
//#else
//	#error Azteck only supports Windows!
//#endif // End of DLL support

#ifdef AZ_DEBUG
	#if defined(AZ_PLATFORM_WINDOWS)
		#define AZ_DEBUGBREAK() __debugbreak()
	#elif defined(AZ_PLATFORM_LINUX)
		#include <signal.h>
		#define AZ_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define AZ_ENABLE_ASSERTS
#else
	#define AZ_DEBUGBREAK()
#endif

#ifdef AZ_DEBUG
	#define AZ_ENABLE_ASSERTS
#endif

#ifdef AZ_ENABLE_ASSERTS
	#define AZ_ASSERT(x, ...) { if(!(x)) { AZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); AZ_DEBUGBREAK(); } }
	#define AZ_CORE_ASSERT(x, ...) { if(!(x)) { AZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); AZ_DEBUGBREAK(); } }
#else
	#define AZ_ASSERT(x, ...)
	#define AZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define AZ_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Azteck
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> createScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> createRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}
