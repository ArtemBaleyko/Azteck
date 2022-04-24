#include "azpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Azteck
{
	OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc)
		: _rendererId(0)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			AZ_CORE_ERROR("{0}", infoLog.data());
			AZ_CORE_ASSERT(false, "Vertex shader compilation failire!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = fragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			AZ_CORE_ERROR("{0}", infoLog.data());
			AZ_CORE_ASSERT(false, "Fragment shader compilation failire!");

			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		_rendererId = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(_rendererId, vertexShader);
		glAttachShader(_rendererId, fragmentShader);

		// Link our program
		glLinkProgram(_rendererId);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(_rendererId, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(_rendererId, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(_rendererId, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(_rendererId);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			AZ_CORE_ERROR("{0}", infoLog.data());
			AZ_CORE_ASSERT(false, "Shader link failire!");

			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(_rendererId, vertexShader);
		glDetachShader(_rendererId, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(_rendererId);
	}

	void OpenGLShader::bind() const
	{
		glUseProgram(_rendererId);
	}

	void OpenGLShader::unBind() const
	{
		glUseProgram(0);
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
}