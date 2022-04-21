#include "azpch.h"
#include "Application.h"
#include "Azteck/Log.h"
#include "Input.h"
#include "Azteck/Renderer/Renderer.h"

#include <glm/glm.hpp>

namespace Azteck
{
	Application* Application::_instance = nullptr;

	Application::Application()
		: _camera(-1.6f, 1.6f, -0.9f, 0.9f)
		,_isRunning(true)
	{
		AZ_CORE_ASSERT(!_instance, "Application already exists");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::Create());
		_window->setEventCallback(AZ_BIND_EVENT_FN(Application::onEvent));

		_imGuiLayer = new ImGuiLayer;
		pushOverlay(_imGuiLayer);

		_vertexArray.reset(VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));

		BufferLayout layout = {
			{ShaderDataType::Float3, "vertices"},
			{ShaderDataType::Float4, "color"},
		};

		vertexBuffer->setLayout(layout);
		_vertexArray->addVertexBuffer(vertexBuffer);

		uint32_t indices[] = { 0, 1, 2 };

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, 3));

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

		_shader.reset(new Shader(vertexSrc, fragmentSrc));
	}

	Application::~Application()
	{
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WindowCloseEvent>(AZ_BIND_EVENT_FN(Application::onWindowClose));

		for (auto it = _layerStack.end(); it != _layerStack.begin();)
		{
			(*--it)->onEvent(e);
			if (e.isHandled())
				break;
		}
	}

	void Application::pushLayer(Layer* layer)
	{
		_layerStack.pushLayer(layer);
		layer->onAttach();
	}

	void Application::pushOverlay(Layer* layer)
	{
		_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		_isRunning = false;
		return true;
	}

	void Application::run()
	{
		while (_isRunning)
		{
			RenderCommand::setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
			RenderCommand::clear();

			Renderer::beginScene(_camera);

			Renderer::submit(_shader, _vertexArray);

			Renderer::endScene();

			for (Layer* layer : _layerStack)
				layer->onUpdate();

			_imGuiLayer->begin();

			for (Layer* layer : _layerStack)
				layer->onImGuiRender();

			_imGuiLayer->end();
			_window->onUpdate();
		}
	}
}
