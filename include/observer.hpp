#pragma once
#include <array>
#include <vector>
#include <functional>
#include <variant>
#include <tuple>
#include "entt/entity/registry.hpp"
#include "universeNode.hpp"
#include "unordered_map"

using eventArgs = std::variant<
	std::tuple<entt::entity, entt::entity, double>, //e, e, m/s
	std::tuple<universeNode*, entt::entity, double>,
	std::tuple<universeNode*, universeNode*, double>
>;

enum class eventType{
	COLLISION_EE,
	COLLISION_NE,
	COLLISION_NN,
	PROJECTILEHIT,
	_SIZE,
};

namespace {
	constexpr const std::array<size_t,(unsigned)eventType::_SIZE> _observertypetable(std::array<std::pair<eventType,eventArgs>,(unsigned)eventType::_SIZE> args){
		std::array<size_t,(unsigned)eventType::_SIZE> result;
		for(auto& val : args){
			result[(unsigned)val.first] = val.second.index();
		}
		return result;
	}
}

constexpr std::array<size_t,(unsigned)eventType::_SIZE> _eventLUT = _observertypetable(std::array<std::pair<eventType,eventArgs>,(unsigned)eventType::_SIZE>{
	std::make_pair(eventType::COLLISION_EE,std::tuple<entt::entity,entt::entity, double>()),
	std::make_pair(eventType::COLLISION_NE,std::tuple<universeNode*,entt::entity, double>()),
	std::make_pair(eventType::COLLISION_NN,std::tuple<universeNode*,universeNode*, double>()),
	std::make_pair(eventType::PROJECTILEHIT,std::tuple<entt::entity,entt::entity, double>())
});


class observer {
public:
	static void registerObserver(eventType t, std::function<void(eventArgs args)> f, void* owner);
	static void deleteObserver(eventType t, void* owner);

	template <eventType E,typename T>
	static void sendEvent(T args)
	{
		static_assert(eventArgs(T()).index()==_eventLUT[(unsigned)E]);
		for(auto& sub : _subscribers[(unsigned)E]){
			sub.second(eventArgs(args));
		}
	}

private:
	static std::array<std::unordered_map<void*,std::function<void(eventArgs args)>>,(unsigned)eventType::_SIZE> _subscribers;
};
