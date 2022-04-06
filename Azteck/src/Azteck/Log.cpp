#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Azteck
{
	std::shared_ptr<spdlog::logger> Log::_coreLogger;
	std::shared_ptr<spdlog::logger> Log::_clientLogger;

	void Log::init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		_coreLogger = spdlog::stdout_color_mt("Azteck");
		_coreLogger->set_level(spdlog::level::trace);

		_clientLogger = spdlog::stdout_color_mt("App");
		_clientLogger->set_level(spdlog::level::trace);
	}
}
