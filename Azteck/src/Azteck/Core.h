#pragma once

#ifdef AZ_PLATFORM_WINDOWS
	#ifdef AZ_BUILD_DLL
		#define AZTECK_API __declspec(dllexport)
	#else
		#define AZTECK_API __declspec(dllimport)
	#endif
#else
	#error Azteck only supports Windows
#endif

#define BIT(x) (1 << x)