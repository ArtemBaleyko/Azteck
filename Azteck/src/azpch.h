#pragma once

#include "Azteck/Core/PlatformDetection.h"

#ifdef AZ_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
		#define NOMINMAX
	#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>

#include "Azteck/Core/Core.h"
#include "Azteck/Core/Log.h"
#include "Azteck/Debug/Instrumentor.h"

#ifdef AZ_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
