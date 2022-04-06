#pragma once

#include "Core.h"

namespace Azteck
{
	class AZTECK_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	};

	// To be defined in client
	Application* createApplication();
}

