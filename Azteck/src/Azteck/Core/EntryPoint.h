#pragma once

#ifdef AZ_PLATFORM_WINDOWS

extern Azteck::Application* Azteck::createApplication();

int main(int argc, char** argv)
{
	Azteck::Log::init();

	auto app = Azteck::createApplication();
	app->run();
	delete app;
}

#endif