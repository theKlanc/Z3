﻿#pragma once
#include <memory>
#include <vector>
#include "fdd.hpp"
#include "json.hpp"
#include "block.hpp"
#include "terrainChunk.hpp"
#include "nodeGenerator.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

using nlohmann::json;

enum nodeType{
	STAR,
	BLACK_HOLE,
	PLANET_GAS,
	PLANET_ROCK,
	ASTEROID,
	SATELLITE_NATURAL,
	SATELLITE_ARTIFICIAL,
	SPACE_STATION,
	SPACESHIP,
};

NLOHMANN_JSON_SERIALIZE_ENUM( nodeType, {
	{STAR,"STAR"},
	{BLACK_HOLE,"BLACK_HOLE"},
	{PLANET_GAS,"PLANET_GAS"},
	{PLANET_ROCK,"PLANET_ROCK"},
	{ASTEROID,"ASTEROID"},
	{SATELLITE_NATURAL,"SATELLITE_NATURAL"},
	{SATELLITE_ARTIFICIAL,"SATELLITE_ARTIFICIAL"},
	{SPACE_STATION,"SPACE_STATION"},
	{SPACESHIP,"SPACESHIP"},
})


class universeNode {
public:
	universeNode():_chunks(config::chunkLoadRadius*config::chunkLoadRadius*config::chunkLoadRadius){}
	block& getBlock(const point3Di &pos);
	void setBlock(block* b, const point3Di &pos);
	void updateChunks(const fdd& playerPos, universeNode* u);

	bool findNodeByID(const unsigned int& id, universeNode*& result);
	bool operator!= (const universeNode& right)const;
	bool operator== (const universeNode& right)const;
	friend void to_json(nlohmann::json &j, const universeNode &f);
	friend void from_json(const json& j, universeNode& f);

  private:
	point3Di chunkFromPos(const fdd& pos);
  
	void iUpdateChunks(const point3Di& localChunk);
	terrainChunk& chunkAt(const point3Di &pos);
	terrainChunk& getChunk(const point3Di &pos);
	void linkChildren();
	fdd getLocalPos(fdd f,universeNode* u) const;


	std::string _name;
	double _mass; // mass in kg
	double _diameter; // diameter in m
	fdd _position;
	fdd _velocity;

	std::vector<terrainChunk> _chunks; // So big, should be on the heap. So fat, too much for the stack.

	nodeType _type;
	std::vector<universeNode> _children;
	universeNode* _parent;
	std::unique_ptr<nodeGenerator> _generator;
	unsigned int _depth;
	unsigned int _ID;
  
};

void to_json(json& j, const universeNode& f);
void from_json(const json& j, universeNode& f);