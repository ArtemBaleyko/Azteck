#include "azpch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Azteck
{
	Ref<Shader> Shader::create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				AZ_CORE_ASSERT(false, "RendererAPI::None is not supported");
				return nullptr;
			}

			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}

	Ref<Shader> Shader::create(const std::string& filepath)
	{
		switch (Renderer::getAPI())
		{
			case RendererAPI::API::None:
			{
				AZ_CORE_ASSERT(false, "RendererAPI::None is not supported");
				return nullptr;
			}

			case RendererAPI::API::OpenGL:
			{
				return std::make_shared<OpenGLShader>(filepath);
			}

			default:
			{
				AZ_CORE_ASSERT(false, "RendererAPI type is unknown");
				return nullptr;
			}
		}
	}

	void ShaderLibrary::add(const Ref<Shader>& shader)
	{
		const std::string& name = shader->getName();
		add(name, shader);
	}

	void ShaderLibrary::add(const std::string& name, const Ref<Shader>& shader)
	{
		AZ_CORE_ASSERT(!exists(name), "Shader with name {0} already exists", name);
		_shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& filepath)
	{
		auto shader = Shader::create(filepath);
		add(shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::create(filepath);
		add(name, shader);

		return shader;
	}

	Ref<Shader> ShaderLibrary::get(const std::string& name) const
	{
		if (!exists(name))
		{
			AZ_CORE_ASSERT(false, "Shader with name {0} was not found", name);
			AZ_CORE_ERROR("Shader with name {0} was not found", name);
			return nullptr;
		}

		return _shaders.find(name)->second;
	}

	bool ShaderLibrary::exists(const std::string& name) const
	{
		return _shaders.find(name) != _shaders.end();
	}
}