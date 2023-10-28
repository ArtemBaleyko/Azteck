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

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				_context->createEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();


		ImGui::Begin("Properties");

		if (_selectedEntity)
		{
			drawComponents(_selectedEntity);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity)
	{
		auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (_selectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;

		bool isOpened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::IsItemClicked())
			_selectedEntity = entity;

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (isOpened)
			ImGui::TreePop();

		if (entityDeleted)
		{
			_context->destroyEntity(entity);
			if (_selectedEntity == entity)
				_selectedEntity = {};
		}
	}

	void SceneHierarchyPanel::drawComponents(Entity entity)
	{
		if (entity.hasComponent<TagComponent>())
		{
			auto& tag = entity.getComponent<TagComponent>().tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
				tag = buffer;
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				_selectedEntity.addComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				_selectedEntity.addComponent<SpriteRenderComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		drawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				drawVec3Control("Position", component.translation);

				glm::vec3 rotation = glm::degrees(component.rotation);
				drawVec3Control("Rotation", rotation);
				component.rotation = glm::radians(rotation);

				drawVec3Control("Scale", component.scale, 1.0f);
			});

		drawComponent<CameraComponent>("Camera", entity, [](auto& component) 
		{
			auto& camera = component.camera;

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentType = projectionTypeStrings[static_cast<int>(camera.getProjectionType())];

			ImGui::Checkbox("Primary", &component.primary);

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

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
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
		});

		drawComponent<SpriteRenderComponent>("Sprite Renderer", entity, [](auto& component) 
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
		});
	}

	template<typename T, typename UIFunction>
	static void SceneHierarchyPanel::drawComponent(const std::string& label, Entity entity, UIFunction func)
	{
		if (entity.hasComponent<T>())
		{
			const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen
				| ImGuiTreeNodeFlags_AllowItemOverlap
				| ImGuiTreeNodeFlags_Framed
				| ImGuiTreeNodeFlags_SpanAvailWidth
				| ImGuiTreeNodeFlags_FramePadding;

			auto& component = entity.getComponent<T>();
			ImVec2 contentRegioAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4, 4 });
			const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();

			bool opened = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegioAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", { lineHeight, lineHeight }))
				ImGui::OpenPopup("ComponentSettings");


			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (opened)
			{
				func(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.removeComponent<T>();
		}
	}

	void SceneHierarchyPanel::drawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

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
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);


		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();


		ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
	}
}
