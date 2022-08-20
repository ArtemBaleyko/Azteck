#include "azpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Azteck
{
	static GLenum shaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		AZ_CORE_ASSERT(false, "Unknown shader type '{0}'", type);
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: _rendererId(0)
		, _name(name)
	{
		AZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources = {
			{GL_VERTEX_SHADER, vertexSrc},
			{GL_FRAGMENT_SHADER, fragmentSrc}
		};

		compile(sources);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: _rendererId(0)
	{
		AZ_PROFILE_FUNCTION();

		std::string source = readFile(filepath);
		auto shaderSources = preProcess(source);
		compile(shaderSources);

		size_t lastSlashPos = filepath.find_last_of("/\\");
		lastSlashPos = lastSlashPos == std::string::npos ? 0 : lastSlashPos + 1;

		size_t lastDot = filepath.rfind('.');
		size_t count = lastDot == std::string::npos ? filepath.size() - lastSlashPos : lastDot - lastSlashPos;
		_name = filepath.substr(lastSlashPos, count);
	}

	OpenGLShader::~OpenGLShader()
	{
		AZ_PROFILE_FUNCTION();

		glDeleteProgram(_rendererId);
	}

	void OpenGLShader::bind() const
	{
		AZ_PROFILE_FUNCTION();

		glUseProgram(_rendererId);
	}

	void OpenGLShader::unBind() const
	{
		AZ_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformMat4(name, value);
	}

	void OpenGLShader::setFloat(const std::string& name, float value)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformFloat(name, value);
	}

	void OpenGLShader::setFloat3(const std::string& name, const glm::vec3& value)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformFloat3(name, value);
	}

	void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformFloat4(name, value);
	}

	void OpenGLShader::setInt(const std::string& name, int value)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformInt(name, value);
	}

	void OpenGLShader::setIntArray(const std::string& name, int* values, uint32_t count)
	{
		AZ_PROFILE_FUNCTION();

		uploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::uploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::uploadUniformFloat(const std::string& name, float value)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::uploadUniformFloat2(const std::string& name, const glm::vec2& values)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform2f(location, values.x, values.y);
	}

	void OpenGLShader::uploadUniformFloat3(const std::string& name, const glm::vec3& values)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform3f(location, values.x, values.y, values.z);
	}

	void OpenGLShader::uploadUniformFloat4(const std::string& name, const glm::vec4& values)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform4f(location, values.x, values.y, values.z, values.w);
	}

	void OpenGLShader::uploadUniformInt(const std::string& name, int value)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::uploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		int location = glGetUniformLocation(_rendererId, name.c_str());
		glUniform1iv(location, count, values);
	}

	std::string OpenGLShader::readFile(const std::string& filepath)
	{
		AZ_PROFILE_FUNCTION();

		std::string result;
		std::ifstream file(filepath, std::ios::in | std::ios::binary);
		
		if (file.is_open())
		{
			file.seekg(0, std::ios::end);
			result.resize(file.tellg());

			file.seekg(0, std::ios::beg);
			file.read(result.data(), result.size());
			file.close();
		}
		else
		{
			AZ_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
	{
		AZ_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0); //Start of shader type declaration line
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			AZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			AZ_CORE_ASSERT(shaderTypeFromString(type), "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			AZ_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[shaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return shaderSources;
	}

	void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		AZ_PROFILE_FUNCTION();

		AZ_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders can be loaded at once");

		GLuint program = glCreateProgram();
		std::array<GLenum, 2> shadersIds;

		size_t shaderIdIdx = 0;
		for (const auto& [type, source] : shaderSources)
		{
			GLuint shader = glCreateShader(type);

			const GLchar* shaderSrc = source.c_str();
			glShaderSource(shader, 1, &shaderSrc, nullptr);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shader);

				AZ_CORE_ERROR("{0}", infoLog.data());
				AZ_CORE_ASSERT(false, "Shader compilation failire!");
				break;
			}

			glAttachShader(program, shader);
			shadersIds[shaderIdIdx++] = shader;
		}

		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);

			for (auto shaderId : shadersIds)
				glDeleteShader(shaderId);

			AZ_CORE_ERROR("{0}", infoLog.data());
			AZ_CORE_ASSERT(false, "Shader link failire!");

			return;
		}

		for (auto shaderId : shadersIds)
		{
			glDetachShader(program, shaderId);
			glDeleteShader(shaderId);
		}

		_rendererId = program;
	}

}