#include <Azteck.h>

#include <glm/gtc/matrix_transform.hpp>
#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
		, _squarePosition(0.0f)
		, _moveSpeed(1.0f)
		, _squareColor(0.2f, 0.3f, 0.8f)
	{
		_vertexArray.reset(Azteck::VertexArray::create());

		//float vertices[] = {
		//	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		//	 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		//};

		float vertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
		};

		Azteck::Ref<Azteck::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Azteck::VertexBuffer::create(vertices, sizeof(vertices)));

		Azteck::BufferLayout layout = {
			{Azteck::ShaderDataType::Float3, "vertices"},
		};

		vertexBuffer->setLayout(layout);
		_vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		Azteck::Ref<Azteck::IndexBuffer> indexBuffer;
		indexBuffer.reset(Azteck::IndexBuffer::create(indices, 6));

		_vertexArray->setIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;									

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;			

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		_shader.reset(Azteck::Shader::create(vertexSrc, fragmentSrc));
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

		if (Azteck::Input::isKeyPressed(AZ_KEY_L))
			_squarePosition.x += _moveSpeed * timestep;
		else if (Azteck::Input::isKeyPressed(AZ_KEY_J))
			_squarePosition.x -= _moveSpeed * timestep;

		if (Azteck::Input::isKeyPressed(AZ_KEY_I))
			_squarePosition.y += _moveSpeed * timestep;
		else if (Azteck::Input::isKeyPressed(AZ_KEY_K))
			_squarePosition.y -= _moveSpeed * timestep;

		Azteck::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Azteck::RenderCommand::clear();

		_camera.setPosition(_cameraPosition);
		_camera.setRotation(_cameraRotation);

		Azteck::Renderer::beginScene(_camera);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), _squarePosition);
		static glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		auto openGLShader = std::dynamic_pointer_cast<Azteck::OpenGLShader>(_shader);
		openGLShader->bind();
		openGLShader->uploadUniformFloat3("u_Color", _squareColor);

		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 20; j++)
			{
				glm::vec3 pos(i * 0.11f, j * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

				Azteck::Renderer::submit(_shader, _vertexArray, transform);
			}
		}

		Azteck::Renderer::endScene();
	}

	void onEvent(Azteck::Event& e) override
	{

	}

	void onImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color", glm::value_ptr(_squareColor));
		ImGui::End();
	}

private:
	Azteck::Ref<Azteck::Shader> _shader;
	Azteck::Ref<Azteck::VertexArray> _vertexArray;

	Azteck::OrthographicCamera _camera;
	glm::vec3 _cameraPosition;
	float _cameraRotation;
	float _cameraMoveSpeed;
	float _cameraRotationSpeed;

	glm::vec3 _squarePosition;
	float _moveSpeed;

	glm::vec3 _squareColor;
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
