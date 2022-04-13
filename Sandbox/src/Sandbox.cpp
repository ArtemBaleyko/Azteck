#include <Azteck.h>

class ExampleLayer : public Azteck::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void onUpdate() override
	{
	}

	void onEvent(Azteck::Event& e) override
	{
	}
};

class Sandbox : public Azteck::Application
{
public:
	Sandbox() 
	{
		pushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

Azteck::Application* Azteck::createApplication()
{
	return new Sandbox();
}
