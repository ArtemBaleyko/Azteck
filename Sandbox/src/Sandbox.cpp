#include <Azteck.h>

class Sandbox : public Azteck::Application
{
public:
	Sandbox() {}
	~Sandbox() {}


};

Azteck::Application* Azteck::createApplication()
{
	return new Sandbox();
}
