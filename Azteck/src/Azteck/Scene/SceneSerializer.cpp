#include "azpch.h"
#include "SceneSerializer.h"

#include "Entity.h"
#include "Components.h"
#include "Azteck/Scripting/ScriptEngine.h"
#include "Azteck/Core/UUID.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

	template<>
	struct convert<Azteck::UUID>
	{
		static Node encode(const Azteck::UUID& uuid)
		{
			Node node;
			node.push_back((uint64_t)uuid);
			return node;
		}

		static bool decode(const Node& node, Azteck::UUID& uuid)
		{
			uuid = node.as<uint64_t>();
			return true;
		}
	};
}

namespace Azteck
{
#define WRITE_SCRIPT_FIELD(FieldType, Type)           \
			case ScriptFieldType::FieldType:          \
				out << scriptField.getValue<Type>();  \
				break

#define READ_SCRIPT_FIELD(FieldType, Type)             \
	case ScriptFieldType::FieldType:                   \
	{                                                  \
		Type data = scriptField["Data"].as<Type>();    \
		fieldInstance.setValue(data);                  \
		break;                                         \
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	static std::string rigidBody2DTypeToString(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
			case Rigidbody2DComponent::BodyType::Static: return "Static";
			case Rigidbody2DComponent::BodyType::Dynamic: return "Dynamic";
			case Rigidbody2DComponent::BodyType::Kinematic: return "Kinematic";
			default:
				break;
		}

		AZ_CORE_ASSERT(false, "Unknown type");
		return {};
	}

	static Rigidbody2DComponent::BodyType rigidBody2DTypeFromString(const std::string& type)
	{
		if (type == "Static") return Rigidbody2DComponent::BodyType::Static;
		if (type == "Dynamic") return Rigidbody2DComponent::BodyType::Dynamic;
		if (type == "Kinematic") return Rigidbody2DComponent::BodyType::Kinematic;

		AZ_CORE_ASSERT(false, "Unknown type");
		return {};
	}

	SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
		: _scene(scene)
	{
	}

	void SceneSerializer::serialize(const std::string& filepath)
	{
		AZ_CORE_ASSERT(_scene, "Scene is nullptr");

		YAML::Emitter out;
		out << YAML::BeginMap;

		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		_scene->_registry.each([&](auto entityID) 
		{
			Entity entity = { entityID, _scene.get() };
			if (!entity)
				return;

			serializeEntity(out, entity);
		});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
		fout.close();
	}

	void SceneSerializer::serializeRuntime(const std::string& filepath)
	{
		AZ_CORE_ASSERT(false, "Not implemented");
	}

	bool SceneSerializer::deserialize(const std::string& filepath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			AZ_CORE_ERROR("Failed to load .hazel file '{0}'\n {1}", filepath, e.what());
			return false;
		}

		if (!data["Scene"])
			return false;

		std::string sceneName = data["Scene"].as<std::string>();
		AZ_CORE_TRACE("Deserializing scene {0}", sceneName);

		auto entities = data["Entities"];
		if (!entities)
		{
			AZ_CORE_WARN("Scene {0} doesn`t contain any entities", sceneName);
			return false;
		}

		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name = "";
			if (auto tagComponent = entity["TagComponent"])
			{
				name = tagComponent["Tag"].as<std::string>();

				AZ_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);
			}

			Entity deserializedEntity = _scene->createEntityWithUUID(uuid, name);

			if (auto transformComponent = entity["TransformComponent"])
			{
				auto& tc = deserializedEntity.getComponent<TransformComponent>();
				tc.translation = transformComponent["Translation"].as<glm::vec3>();
				tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
				tc.scale = transformComponent["Scale"].as<glm::vec3>();
			}

			if (auto cameraComponent = entity["CameraComponent"])
			{
				auto& cc = deserializedEntity.addComponent<CameraComponent>();

				auto& cameraProps = cameraComponent["Camera"];
				cc.camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<int>()));

				cc.camera.setVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.camera.setPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.camera.setPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

				cc.camera.setOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				cc.camera.setOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				cc.camera.setOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

				cc.primary = cameraComponent["Primary"].as<bool>();
				cc.fixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}
		
			if (auto scriptComponent = entity["ScriptComponent"])
			{
				auto& sc = deserializedEntity.addComponent<ScriptComponent>();
				sc.className = scriptComponent["ClassName"].as<std::string>();

				if (auto scriptFields = scriptComponent["ScriptFields"])
				{
					Ref<ScriptClass> entityClass = ScriptEngine::getEntityClass(sc.className);
					AZ_CORE_ASSERT(entityClass, "Entity class is nullptr");
					const auto& fields = entityClass->getFields();
					auto& entityFields = ScriptEngine::getScriptFieldMap(deserializedEntity);

					for (auto scriptField : scriptFields)
					{
						std::string name = scriptField["Name"].as<std::string>();
						std::string typeString = scriptField["Type"].as<std::string>();
						ScriptFieldType type = Utils::scriptFieldTypeFromString(typeString);

						ScriptFieldInstance& fieldInstance = entityFields[name];

						if (fields.find(name) == fields.end())
							continue;

						fieldInstance.field = fields.at(name);

						switch (type)
						{
							READ_SCRIPT_FIELD(Float, float);
							READ_SCRIPT_FIELD(Double, double);
							READ_SCRIPT_FIELD(Bool, bool);
							READ_SCRIPT_FIELD(Char, char);
							READ_SCRIPT_FIELD(Byte, int8_t);
							READ_SCRIPT_FIELD(Short, int16_t);
							READ_SCRIPT_FIELD(Int, int32_t);
							READ_SCRIPT_FIELD(Long, int64_t);
							READ_SCRIPT_FIELD(UByte, uint8_t);
							READ_SCRIPT_FIELD(UShort, uint16_t);
							READ_SCRIPT_FIELD(UInt, uint32_t);
							READ_SCRIPT_FIELD(ULong, uint64_t);
							READ_SCRIPT_FIELD(Vector2, glm::vec2);
							READ_SCRIPT_FIELD(Vector3, glm::vec3);
							READ_SCRIPT_FIELD(Vector4, glm::vec4);
							READ_SCRIPT_FIELD(Entity, UUID);
						}
					}
				}
			}

			if (auto spriteRenderComponent = entity["SpriteRendererComponent"])
			{
				auto& src = deserializedEntity.addComponent<SpriteRendererComponent>();
				src.color = spriteRenderComponent["Color"].as<glm::vec4>();

				if (spriteRenderComponent["TilingFactor"])
					src.tilingFactor = spriteRenderComponent["TilingFactor"].as<float>();

				if (spriteRenderComponent["TexturePath"])
				{
					std::string texturePath = spriteRenderComponent["TexturePath"].as<std::string>();
					src.texture = Texture2D::create(texturePath);
				}
			}

			if (auto circleRenderComponent = entity["CircleRendererComponent"])
			{
				auto& src = deserializedEntity.addComponent<CircleRendererComponent>();
				src.color = circleRenderComponent["Color"].as<glm::vec4>();
				src.thickness = circleRenderComponent["Thickness"].as<float>();
				src.fade = circleRenderComponent["Fade"].as<float>();
			}

			if (auto rigidbody2DComponent = entity["Rigidbody2DComponent"])
			{
				auto& rb2d = deserializedEntity.addComponent<Rigidbody2DComponent>();
				rb2d.type = rigidBody2DTypeFromString(rigidbody2DComponent["BodyType"].as<std::string>());
				rb2d.fixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			}

			if (auto boxCollider2DComponent = entity["BoxCollider2DComponent"])
			{
				auto& bc2d = deserializedEntity.addComponent<BoxCollider2DComponent>();
				bc2d.offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
				bc2d.size = boxCollider2DComponent["Size"].as<glm::vec2>();
				bc2d.density = boxCollider2DComponent["Density"].as<float>();
				bc2d.friction = boxCollider2DComponent["Friction"].as<float>();
				bc2d.restitution = boxCollider2DComponent["Restitution"].as<float>();
				bc2d.restitutionThreshold = boxCollider2DComponent["RestitutionThreshold"].as<float>();
			}

			if (auto circleCollider2DComponent = entity["CircleCollider2DComponent"])
			{
				auto& cc2d = deserializedEntity.addComponent<CircleCollider2DComponent>();
				cc2d.offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
				cc2d.radius = circleCollider2DComponent["Radius"].as<float>();
				cc2d.density = circleCollider2DComponent["Density"].as<float>();
				cc2d.friction = circleCollider2DComponent["Friction"].as<float>();
				cc2d.restitution = circleCollider2DComponent["Restitution"].as<float>();
				cc2d.restitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
			}
		}

		return true;
	}

	bool SceneSerializer::deserializeRuntime(const std::string& filepath)
	{
		AZ_CORE_ASSERT(false, "Not implemented");
		return false;
	}

	static void serializeEntity(YAML::Emitter& out, Entity entity)
	{
		AZ_CORE_ASSERT(entity.hasComponent<IDComponent>(), "Entity must have a UUID");

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.getUUID(); // ID

		if (entity.hasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;

			auto& tag = entity.getComponent<TagComponent>().tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;

			auto& transformComponent = entity.getComponent<TransformComponent>();
			out << YAML::Key << "Translation" << YAML::Value << transformComponent.translation;
			out << YAML::Key << "Rotation" << YAML::Value << transformComponent.rotation;
			out << YAML::Key << "Scale" << YAML::Value << transformComponent.scale;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.getComponent<CameraComponent>();
			auto& camera = cameraComponent.camera;

			out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
			out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.getProjectionType());
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.getVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.getPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.getPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.getOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.getOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.getOrthographicFarClip();
			out << YAML::EndMap;

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.fixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<ScriptComponent>())
		{
			auto& scriptComponent = entity.getComponent<ScriptComponent>();

			out << YAML::Key << "ScriptComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "ClassName" << YAML::Value << scriptComponent.className;

			if (Ref<ScriptClass> entityClass = ScriptEngine::getEntityClass(scriptComponent.className))
			{
				const auto& fields = entityClass->getFields();
				if (!fields.empty())
				{
					out << YAML::Key << "ScriptFields" << YAML::Value;

					auto& entityFields = ScriptEngine::getScriptFieldMap(entity);

					out << YAML::BeginSeq;
					for (const auto& [name, field] : fields)
					{
						if (entityFields.find(name) == entityFields.end())
							continue;

						out << YAML::BeginMap;
						out << YAML::Key << "Name" << YAML::Value << name;
						out << YAML::Key << "Type" << YAML::Value << Utils::scriptFieldTypeToString(field.type);

						out << YAML::Key << "Data" << YAML::Value;
						ScriptFieldInstance& scriptField = entityFields.at(name);

						switch (field.type)
						{
							WRITE_SCRIPT_FIELD(Float, float);
							WRITE_SCRIPT_FIELD(Double, double);
							WRITE_SCRIPT_FIELD(Bool, bool);
							WRITE_SCRIPT_FIELD(Char, char);
							WRITE_SCRIPT_FIELD(Byte, int8_t);
							WRITE_SCRIPT_FIELD(Short, int16_t);
							WRITE_SCRIPT_FIELD(Int, int32_t);
							WRITE_SCRIPT_FIELD(Long, int64_t);
							WRITE_SCRIPT_FIELD(UByte, uint8_t);
							WRITE_SCRIPT_FIELD(UShort, uint16_t);
							WRITE_SCRIPT_FIELD(UInt, uint32_t);
							WRITE_SCRIPT_FIELD(ULong, uint64_t);
							WRITE_SCRIPT_FIELD(Vector2, glm::vec2);
							WRITE_SCRIPT_FIELD(Vector3, glm::vec3);
							WRITE_SCRIPT_FIELD(Vector4, glm::vec4);
							WRITE_SCRIPT_FIELD(Entity, UUID);
						}
						out << YAML::EndMap;
					}
					out << YAML::EndSeq;
				}
			}

			out << YAML::EndMap;
		}

		if (entity.hasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& spriteRendererComponent = entity.getComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.color;
			out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.tilingFactor;

			if (spriteRendererComponent.texture)
				out << YAML::Key << "TexturePath" << YAML::Value << spriteRendererComponent.texture->getPath();

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& circleRendererComponent = entity.getComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << circleRendererComponent.color;
			out << YAML::Key << "Thickness" << YAML::Value << circleRendererComponent.thickness;
			out << YAML::Key << "Fade" << YAML::Value << circleRendererComponent.fade;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& rb2dComponent = entity.getComponent<Rigidbody2DComponent>();
			out << YAML::Key << "BodyType" << YAML::Value << rigidBody2DTypeToString(rb2dComponent.type);
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2dComponent.fixedRotation;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& bc2dComponent = entity.getComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc2dComponent.offset;
			out << YAML::Key << "Size" << YAML::Value << bc2dComponent.size;
			out << YAML::Key << "Density" << YAML::Value << bc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << bc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc2dComponent.restitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.hasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& cc2dComponent = entity.getComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc2dComponent.offset;
			out << YAML::Key << "Radius" << YAML::Value << cc2dComponent.radius;
			out << YAML::Key << "Density" << YAML::Value << cc2dComponent.density;
			out << YAML::Key << "Friction" << YAML::Value << cc2dComponent.friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc2dComponent.restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc2dComponent.restitutionThreshold;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}
}
