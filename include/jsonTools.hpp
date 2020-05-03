#pragma once
#include <vector>
#include <string>
#include "json.hpp"
#include "entt/entity/fwd.hpp"
#include "HI2.hpp"
#include "fdd.hpp"
#include "interactable.hpp"

enum componentType{
	NAME,
	POSITION,
	VELOCITY,
	BODY,
	DRAWABLE,
	BRAIN,
};

NLOHMANN_JSON_SERIALIZE_ENUM( componentType, {
	{NAME,"NAME"},
	{POSITION,"POSITION"},
	{VELOCITY,"VELOCITY"},
	{BODY,"BODY"},
	{DRAWABLE,"DRAWABLE"},
	{BRAIN,"BRAIN"},
})

enum interactableType{
	BLOCK_SWITCH,
	NODE_CONTROLLER
};

NLOHMANN_JSON_SERIALIZE_ENUM( interactableType, {
	{BLOCK_SWITCH,"BLOCK_SWITCH"},
	{NODE_CONTROLLER,"NODE_CONTROLLER"},
})

enum entityTag{
	PLAYER,
	CAMERA,
};

NLOHMANN_JSON_SERIALIZE_ENUM( entityTag, {
	{PLAYER,"PLAYER"},
	{CAMERA,"CAMERA"},
})


struct jsonComponent
{
	
};

struct jsonEntity
{
	std::vector<std::string> tags;
	std::vector<jsonComponent>  components;
};

namespace HI2{
	void to_json(nlohmann::json& j, const HI2::Color& c);
	void from_json(const nlohmann::json& j, HI2::Color& c);
}

void to_json(nlohmann::json& j, const entt::registry& registry);
void from_json(const nlohmann::json& j, entt::registry& registry);


void to_json(nlohmann::json& j, const point3Dd& p);
void from_json(const nlohmann::json& j, point3Dd& p);

void to_json(nlohmann::json& j, const point3Di& p);
void from_json(const nlohmann::json& j, point3Di& p);



std::unique_ptr<interactable> getInteractableFromJson(const nlohmann::json& j);