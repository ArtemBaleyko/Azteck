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
		//AZ_INFO("ExampleLayer::onUpdate");
	}

	void onEvent(Azteck::Event& e) override
	{
		//AZ_TRACE("{0}", e);
	}
};

class Sandbox : public Azteck::Application
{
public:
	Sandbox() 
	{
		pushLayer(new ExampleLayer());
		pushOverlay(new Azteck::ImGuiLayer());
	}

	~Sandbox() {}
};

Azteck::Application* Azteck::createApplication()
{
	return new Sandbox();
}
