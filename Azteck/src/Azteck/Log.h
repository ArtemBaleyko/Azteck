#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Azteck
{
	class AZTECK_API Log
	{
	public:
		static void init();

		inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return _coreLogger; }
		inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return _clientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> _coreLogger;
		static std::shared_ptr<spdlog::logger> _clientLogger;
	};
}

// Core log macros
#define AZ_CORE_FATAL(...)	 ::Azteck::Log::getCoreLogger()->fatal(__VA_ARGS__)
#define AZ_CORE_ERROR(...)	 ::Azteck::Log::getCoreLogger()->error(__VA_ARGS__)
#define AZ_CORE_WARN(...)	 ::Azteck::Log::getCoreLogger()->warn(__VA_ARGS__)
#define AZ_CORE_INFO(...)	 ::Azteck::Log::getCoreLogger()->info(__VA_ARGS__)
#define AZ_CORE_TRACE(...)	 ::Azteck::Log::getCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define AZ_FATAL(...) ::Azteck::Log::getClientLogger()->fatal(__VA_ARGS__)
#define AZ_ERROR(...) ::Azteck::Log::getClientLogger()->error(__VA_ARGS__)
#define AZ_WARN(...)  ::Azteck::Log::getClientLogger()->warn(__VA_ARGS__)
#define AZ_INFO(...)  ::Azteck::Log::getClientLogger()->info(__VA_ARGS__)
#define AZ_TRACE(...) ::Azteck::Log::getClientLogger()->trace(__VA_ARGS__)
