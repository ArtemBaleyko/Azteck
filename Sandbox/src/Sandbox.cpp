#include <Azteck.h>

class ExampleLayer : public Azteck::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
		, _camera(-1.6f, 1.6f, -0.9f, 0.9f)
		, _cameraPosition(0.0f)
		, _cameraMoveSpeed(5.0f)
		, _cameraRotation(0.0f)
		, _cameraRotationSpeed(180.0f)
	{
		_vertexArray.reset(Azteck::VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Azteck::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Azteck::VertexBuffer::create(vertices, sizeof(vertices)));

		Azteck::BufferLayout layout = {
			{Azteck::ShaderDataType::Float3, "vertices"},
			{Azteck::ShaderDataType::Float4, "color"},
		};

		vertexBuffer->setLayout(layout);
		_vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2 };

		std::shared_ptr<Azteck::IndexBuffer> indexBuffer;
		indexBuffer.reset(Azteck::IndexBuffer::create(indices, 3));

		_vertexArray->setIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;					
			layout(location = 1) in vec4 a_Color;					

			uniform mat4 u_viewProjection;

			out vec3 v_Position;
			out vec4 v_Color;
			
			void main()
			{
				v_Color = a_Color;
				v_Position = a_Position;
				gl_Position = u_viewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;			

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = v_Color;
			}
		)";

		_shader.reset(new Azteck::Shader(vertexSrc, fragmentSrc));
	}

	void onUpdate(Azteck::Timestep timestep) override
	{
		AZ_TRACE("Delta time: {0}s ({1}ms)", timestep, timestep.getMilliseconds());

		if (Azteck::Input::isKeyPressed(AZ_KEY_LEFT))
			_cameraPosition.x -= _cameraMoveSpeed * timestep;
		else if (Azteck::Input::isKeyPressed(AZ_KEY_RIGHT))
			_cameraPosition.x += _cameraMoveSpeed * timestep;

		if (Azteck::Input::isKeyPressed(AZ_KEY_UP))
			_cameraPosition.y += _cameraMoveSpeed * timestep;
		else if (Azteck::Input::isKeyPressed(AZ_KEY_DOWN))
			_cameraPosition.y -= _cameraMoveSpeed * timestep;

		if (Azteck::Input::isKeyPressed(AZ_KEY_A))
			_cameraRotation += _cameraRotationSpeed * timestep;
		else if (Azteck::Input::isKeyPressed(AZ_KEY_D))
			_cameraRotation -= _cameraRotationSpeed * timestep;

		Azteck::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Azteck::RenderCommand::clear();

		_camera.setPosition(_cameraPosition);
		_camera.setRotation(_cameraRotation);

		Azteck::Renderer::beginScene(_camera);

		Azteck::Renderer::submit(_shader, _vertexArray);

		Azteck::Renderer::endScene();
	}

	void onEvent(Azteck::Event& e) override
	{

	}

private:
	std::shared_ptr<Azteck::Shader> _shader;
	std::shared_ptr<Azteck::VertexArray> _vertexArray;

	Azteck::OrthographicCamera _camera;
	glm::vec3 _cameraPosition;
	float _cameraRotation;
	float _cameraMoveSpeed;
	float _cameraRotationSpeed;
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
