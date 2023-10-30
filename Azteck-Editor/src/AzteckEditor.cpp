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
		AzteckEditor(ApplicationCommandLineArgs args) : Application("Editor", args)
		{
			pushLayer(new EditorLayer());
		}

		~AzteckEditor()
		{
		}
	};

	Application* createApplication(ApplicationCommandLineArgs args)
	{
		return new AzteckEditor(args);
	}
}
