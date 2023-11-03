#pragma once

#include "Azteck/Core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Azteck
{
	class Log
	{
	public:
		static void init();

		inline static Ref<spdlog::logger>& getCoreLogger() { return _coreLogger; }
		inline static Ref<spdlog::logger>& getClientLogger() { return _clientLogger; }
	private:
		static Ref<spdlog::logger> _coreLogger;
		static Ref<spdlog::logger> _clientLogger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define AZ_CORE_CRITICAL(...)	 ::Azteck::Log::getCoreLogger()->critical(__VA_ARGS__)
#define AZ_CORE_ERROR(...)	 ::Azteck::Log::getCoreLogger()->error(__VA_ARGS__)
#define AZ_CORE_WARN(...)	 ::Azteck::Log::getCoreLogger()->warn(__VA_ARGS__)
#define AZ_CORE_INFO(...)	 ::Azteck::Log::getCoreLogger()->info(__VA_ARGS__)
#define AZ_CORE_TRACE(...)	 ::Azteck::Log::getCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define AZ_CRITICAL(...) ::Azteck::Log::getClientLogger()->critical(__VA_ARGS__)
#define AZ_ERROR(...) ::Azteck::Log::getClientLogger()->error(__VA_ARGS__)
#define AZ_WARN(...)  ::Azteck::Log::getClientLogger()->warn(__VA_ARGS__)
#define AZ_INFO(...)  ::Azteck::Log::getClientLogger()->info(__VA_ARGS__)
#define AZ_TRACE(...) ::Azteck::Log::getClientLogger()->trace(__VA_ARGS__)
