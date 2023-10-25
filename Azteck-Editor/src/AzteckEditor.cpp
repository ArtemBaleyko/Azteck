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
		AzteckEditor() : Application("Editor")
		{
			pushLayer(new EditorLayer());
		}

		~AzteckEditor()
		{
		}
	};

	Application* createApplication()
	{
		return new AzteckEditor();
	}
}
