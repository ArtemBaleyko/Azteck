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
		, _cameraController(1280.0f / 720.0f, true)
		, _squareColor(0.2f, 0.3f, 0.8f)
	{
		_vertexArray = Azteck::VertexArray::create();

		//float vertices[] = {
		//	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		//	 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		//};

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		};

		Azteck::Ref<Azteck::VertexBuffer> vertexBuffer;
		vertexBuffer = Azteck::VertexBuffer::create(vertices, sizeof(vertices));

		Azteck::BufferLayout layout = {
			{Azteck::ShaderDataType::Float3, "a_Position"},
			{Azteck::ShaderDataType::Float2, "a_TexCoord"}
		};

		vertexBuffer->setLayout(layout);
		_vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		Azteck::Ref<Azteck::IndexBuffer> indexBuffer;
		indexBuffer = Azteck::IndexBuffer::create(indices, 6);

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

		_shader = Azteck::Shader::create("flat", vertexSrc, fragmentSrc);

		auto textureShader = _shaderLibrary.load("assets/shaders/Texture.glsl");
		_texture = Azteck::Texture2D::create("assets/textures/checkerboard.png");

		auto openGLTextureShader = std::dynamic_pointer_cast<Azteck::OpenGLShader>(textureShader);
		openGLTextureShader->bind();
		openGLTextureShader->uploadUniformInt("u_Texture", 0);
	}

	void onUpdate(Azteck::Timestep timestep) override
	{
		_cameraController.onUpdate(timestep);

		Azteck::RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		Azteck::RenderCommand::clear();

		Azteck::Renderer::beginScene(_cameraController.getCamera());

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

		auto textureShader = _shaderLibrary.get("Texture");

		_texture->bind();
		Azteck::Renderer::submit(textureShader, _vertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Azteck::Renderer::endScene();
	}

	void onEvent(Azteck::Event& e) override
	{
		_cameraController.onEvent(e);
	}

	void onImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Color", glm::value_ptr(_squareColor));
		ImGui::End();
	}

private:
	Azteck::ShaderLibrary _shaderLibrary;

	Azteck::Ref<Azteck::Shader> _shader;
	Azteck::Ref<Azteck::VertexArray> _vertexArray;
	Azteck::Ref<Azteck::Texture2D> _texture;

	Azteck::OrthographicCameraController _cameraController;

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
