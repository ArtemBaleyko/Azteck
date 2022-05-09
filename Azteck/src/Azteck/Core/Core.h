#pragma once

#include <memory>

#ifdef AZ_PLATFORM_WINDOWS
#if AZ_DYNAMIC_LINK
	#ifdef AZ_BUILD_DLL
		#define AZTECK_API __declspec(dllexport)
	#else
		#define AZTECK_API __declspec(dllimport)
	#endif
#else
	#define AZTECK_API
#endif
#else
	#error Azteck only supports Windows
#endif

#ifdef AZ_DEBUG
	#define AZ_ENABLE_ASSERTS
#endif

#ifdef AZ_ENABLE_ASSERTS
	#define AZ_ASSERT(x, ...) { if(!(x)) { AZ_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define AZ_CORE_ASSERT(x, ...) { if(!(x)) { AZ_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AZ_ASSERT(x, ...)
	#define AZ_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define AZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

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