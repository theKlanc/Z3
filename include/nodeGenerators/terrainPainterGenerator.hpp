#pragma once
#include "nodeGenerator.hpp"
#include "terrainChunk.hpp"

class terrainSection
{
public:
	terrainSection(double noise, int sectionWidth, baseBlock& b, baseBlock* surfaceBlock = nullptr);
	double getNoiseCeiling() const;
	double getSectionWidth() const;
	baseBlock& getBlock() const;
	baseBlock* getSurfaceBlock() const;


	friend void to_json(nlohmann::json &j, const terrainSection &tp);
	friend void from_json(const json& j, terrainSection &tp);
private:
	double _noiseCeiling;
	int _sectionWidth;
	baseBlock& _block;
	baseBlock* _surfaceBlock;

};

class terrainPainter
{
public:
	baseBlock& getBlock(int height, double noise) const;
	baseBlock& getBlock(int height) const;
	void addSection(terrainSection s);
	void setEmptyBlock(baseBlock* emptyBlock);

	int getHeight(const double& noise) const;


	friend void to_json(nlohmann::json &j, const terrainPainter &tp);
	friend void from_json(const json& j, terrainPainter &tp);
private:
	baseBlock* _emptyBlock;
	std::vector<terrainSection> _terrainList;
};

class terrainPainterGenerator : public nodeGenerator{
public:
	terrainPainterGenerator(){}
	terrainPainterGenerator(unsigned int s, unsigned diameter);
	virtual ~terrainPainterGenerator(){}
	terrainChunk getChunk(const point3Di& p)const override;
	int getHeight(const point2D &p)const override;

	friend void to_json(nlohmann::json &j, const terrainPainterGenerator &tpg);
	friend void from_json(const json& j, terrainPainterGenerator &tpg);
private:
	unsigned _diameter;
	unsigned _liquidLevel;
	unsigned _liquidID;
	unsigned _emptyBlockID;
	terrainPainter _terrainPainter;

	
	void fillLiquid(terrainChunk& c, const point3Di p, int liquidLevel) const; // fill chunk c with water on air blocks below waterLevel(nodeHeight), p is the chunk position in chunkSpace
	void populateTrees(terrainChunk& c, const point3Di p, int liquidLevel) const; // populate chunk c with trees on the surface, p is the chunk position in chunkSpace
	void placeTree(terrainChunk& c, const point3Di p) const; //place a tree on chunk c on position p (chunkPosition)
};



void to_json(nlohmann::json &j, const terrainSection &ts);
void from_json(const json& j, terrainSection &ts);
void to_json(nlohmann::json &j, const terrainPainter &tp);
void from_json(const json& j, terrainPainter &tp);
void to_json(nlohmann::json &j, const terrainPainterGenerator &tpg);
void from_json(const json& j, terrainPainterGenerator &tpg);