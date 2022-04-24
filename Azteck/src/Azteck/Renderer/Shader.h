#pragma once

namespace Azteck
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void bind() const = 0;
		virtual void unBind() const = 0;

		static Shader* create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};
}

