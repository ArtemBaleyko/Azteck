#pragma once

namespace Azteck
{
	class ScriptEngine
	{
	public:
		static void init();
		static void shutdown();

	private:
		static void initMono();
		static void shutdownMono();
	};
}
