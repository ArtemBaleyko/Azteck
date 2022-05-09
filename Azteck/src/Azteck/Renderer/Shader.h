#pragma once

#include <glm/glm.hpp>

namespace Azteck
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unBind() const = 0;

		virtual void setMat4(const std::string& name, const glm::mat4& value) = 0;
		virtual void setFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void setFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void setInt(const std::string& name, int value) = 0;

		virtual const std::string& getName() const = 0;

		static Ref<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> create(const std::string& filepath);
	};

	class ShaderLibrary
	{
	public:
		void add(const Ref<Shader>& shader);
		void add(const std::string& name, const Ref<Shader>& shader);
		Ref<Shader> load(const std::string& filepath);
		Ref<Shader> load(const std::string& name, const std::string& filepath);

		Ref<Shader> get(const std::string& name) const;
		bool exists(const std::string& name) const;

	private:
		std::unordered_map<std::string, Ref<Shader>> _shaders;
	};
}

