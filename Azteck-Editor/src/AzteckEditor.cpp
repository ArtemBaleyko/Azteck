#include <Azteck.h>

#include <glm/gtc/matrix_transform.hpp>
#include "Platform/OpenGL/OpenGLShader.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Azteck/Core/EntryPoint.h"
#include "EditorLayer.h"

namespace Azteck 
{
	class AzteckEditor : public Application
	{
	public:
		AzteckEditor(ApplicationSpecification spec) 
			: Application(spec)
		{
			pushLayer(new EditorLayer());
		}

		~AzteckEditor()
		{
		}
	};

	Application* createApplication(ApplicationCommandLineArgs args)
	{
		ApplicationSpecification spec;
		spec.name = "Editor";
		spec.args = args;

		return new AzteckEditor(spec);
	}
}
