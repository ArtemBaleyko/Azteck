#pragma once

#include "Azteck/Renderer/Shader.h"

#include <glm/glm.hpp>

using GLenum = unsigned int;

namespace Azteck
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& filepath);
		virtual ~OpenGLShader();

		void bind() const override;
		void unBind() const override;

		void setMat4(const std::string& name, const glm::mat4& value) override;
		void setFloat(const std::string& name, float value) override;
		void setFloat3(const std::string& name, const glm::vec3& value) override;
		void setFloat4(const std::string& name, const glm::vec4& value) override;
		void setInt(const std::string& name, int value) override;

		void setIntArray(const std::string& name, int* values, uint32_t count) override;

		inline const std::string& getName() const override { return _name; };

		void uploadUniformMat3(const std::string& name, const glm::mat3& matrix);
		void uploadUniformMat4(const std::string& name, const glm::mat4& matrix);

		void uploadUniformFloat(const std::string& name, float value);
		void uploadUniformFloat2(const std::string& name, const glm::vec2& values);
		void uploadUniformFloat3(const std::string& name, const glm::vec3& values);
		void uploadUniformFloat4(const std::string& name, const glm::vec4& values);

		void uploadUniformInt(const std::string& name, int value);

		void uploadUniformIntArray(const std::string& name, int* values, uint32_t count);

	private:
		std::string readFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> preProcess(const std::string& source);
		void compile(const std::unordered_map<GLenum, std::string>& shaderSources);

	private:
		uint32_t _rendererId;
		std::string _name;
	};

}