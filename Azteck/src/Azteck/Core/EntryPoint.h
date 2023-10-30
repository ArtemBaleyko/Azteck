#pragma once
#include "Azteck/Core/Core.h"
#include "Azteck/Core/Application.h"

#ifdef AZ_PLATFORM_WINDOWS

extern Azteck::Application* Azteck::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{
	Azteck::Log::init();

	AZ_PROFILE_BEGIN_SESSION("Startup", "Startup_Profile.json");
	auto app = Azteck::createApplication({argc, argv});
	AZ_PROFILE_END_SESSION();

	AZ_PROFILE_BEGIN_SESSION("Run", "Runtime_Profile.json");
	app->run();
	AZ_PROFILE_END_SESSION();

	AZ_PROFILE_BEGIN_SESSION("Shutdown", "Shutdown_Profile.json");
	delete app;
	AZ_PROFILE_END_SESSION();
}

#endif
