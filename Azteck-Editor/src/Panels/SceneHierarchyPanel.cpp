#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>


#include <glm/gtc/type_ptr.hpp>

#include "Azteck/Scene/Components.h"

namespace Azteck
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		setContext(context);
	}

	void SceneHierarchyPanel::setContext(const Ref<Scene>& context)
	{
		_context = context;
	}

	void SceneHierarchyPanel::onImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		_context->_registry.each([&](auto entityID)
		{
			Entity entity = { entityID, _context.get() };
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			_selectedEntity = {};

		ImGui::End();


		ImGui::Begin("Properties");

		if (_selectedEntity)
			drawComponents(_selectedEntity);

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (_selectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			_selectedEntity = entity;

		if (isOpened)
			ImGui::TreePop();
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
				tag = buffer;
		}

		if (entity.hasComponent<TransformComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				auto& transformComponent = entity.getComponent<TransformComponent>();
				//ImGui::DragFloat3("Position", glm::value_ptr(transformComponent.translation), 0.1f);
				drawVec3Control("Position", transformComponent.translation);

				glm::vec3 rotation = glm::degrees(transformComponent.rotation);
				drawVec3Control("Rotation", rotation);
				transformComponent.rotation = glm::radians(rotation);

				drawVec3Control("Scale", transformComponent.scale, 1.0f);

				ImGui::TreePop();
			}
		}


		if (entity.hasComponent<CameraComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				auto& cameraComponent = entity.getComponent<CameraComponent>();
				auto& camera = cameraComponent.camera;

				const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
				const char* currentType = projectionTypeStrings[static_cast<int>(camera.getProjectionType())];

				ImGui::Checkbox("Primary", &cameraComponent.primary);

				if (ImGui::BeginCombo("Projection", currentType))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentType == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentType = projectionTypeStrings[i];
							camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(i));
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.getOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize, 0.1f))
						camera.setOrthographicSize(orthoSize);

					float nearClip = camera.getOrthographicNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
						camera.setOrthographicNearClip(nearClip);

					float farClip = camera.getOrthographicFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip, 0.1f))
						camera.setOrthographicFarClip(farClip);

					ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.fixedAspectRatio);
				}
				else
				{
					float fov = glm::degrees(camera.getVerticalFOV());
					if (ImGui::DragFloat("FOV", &fov, 0.1f))
						camera.setVerticalFOV(fov);

					float nearClip = camera.getPerspectiveNearClip();
					if (ImGui::DragFloat("Near Clip", &nearClip, 0.1f))
						camera.setPerspectiveNearClip(nearClip);

					float farClip = camera.getPerspectiveFarClip();
					if (ImGui::DragFloat("Far Clip", &farClip, 0.1f))
						camera.setPerspectiveFarClip(farClip);
				}

				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<SpriteRenderComponent>())
		{
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRenderComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
			{
				auto& color = entity.getComponent<SpriteRenderComponent>().color;
				ImGui::ColorEdit4("Color", glm::value_ptr(color));

				ImGui::TreePop();
			}
		}
	}

	void SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGui::PushID(label.c_str());
		ImGui::Columns(2);

		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };


		ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
		if (ImGui::Button("x", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
		if (ImGui::Button("y", buttonSize))
			values.x = resetValue;
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
		if (ImGui::Button("z", buttonSize))
			values.z = resetValue;
		ImGui::PopStyleColor(3);


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}
