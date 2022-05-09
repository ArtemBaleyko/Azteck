#pragma once

#include <Azteck.h>

class Sandbox2D : public Azteck::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	void onAttach() override;
	void onDetach() override;

	void onUpdate(Azteck::Timestep timestep) override;
	void onEvent(Azteck::Event& e) override;
	void onImGuiRender() override;

private:
	Azteck::OrthographicCameraController _cameraController;

	// Temporary
	Azteck::Ref<Azteck::Texture2D> _texture;
	glm::vec4 _squareColor;
};