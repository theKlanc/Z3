#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>
#include <iostream>
#include "states/state_playing.hpp"

terrainChunk::~terrainChunk()
{
	//if (_collisionBody != nullptr) 
	//{
	//	Services::physicsMutex.lock();
	//	Services::collisionWorld->destroyCollisionBody(_collisionBody);
	//	_collisionBody = nullptr;
	//	Services::physicsMutex.unlock();
	//}
}

terrainChunk::terrainChunk(const point3Di& p) : _indexedPosition(p), _loaded(false),
_blocks(config::chunkSize* config::chunkSize* config::chunkSize, { &baseBlock::terrainTable[0],UP }),
_colliders(config::chunkSize* config::chunkSize* config::chunkSize, nullptr)
{
	Services::physicsMutex.lock();
	{
		_collisionBody = (Services::collisionWorld->createCollisionBody(rp3d::Transform{{0,0,0},rp3d::Quaternion::identity()}));
	}
	Services::physicsMutex.unlock();
}

metaBlock& terrainChunk::getBlock(const point3Di& p) {
	int x = p.x % config::chunkSize;
	if (x < 0)
		x += config::chunkSize;
	int y = p.y % config::chunkSize;
	if (y < 0)
		y += config::chunkSize;
	int z = p.z % config::chunkSize;
	if (z < 0)
		z += config::chunkSize;
	return _blocks[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z];
}

void terrainChunk::clearDirtyFlag()
{
	_dirty = false;
}

void terrainChunk::setBlock(metaBlock b, const point3Di& p) {
	_dirty = true;
	int x = p.x % config::chunkSize;
	if (x < 0)
		x += config::chunkSize;
	int y = p.y % config::chunkSize;
	if (y < 0)
		y += config::chunkSize;
	int z = p.z % config::chunkSize;
	if (z < 0)
		z += config::chunkSize;
	_blocks[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] = b;
	Services::physicsMutex.lock();
	if (_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] != nullptr)
	{
		_collisionBody->removeCollisionShape(_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z]);
		_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] = nullptr;
	}
	if (b.base->solid)
	{
		_colliders[(x * config::chunkSize * config::chunkSize) + (y * config::chunkSize) + z] = _collisionBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(x + 0.5),(rp3d::decimal)(y + 0.5),(rp3d::decimal)(z + 0.5)},rp3d::Quaternion::identity() });
	}
	Services::physicsMutex.unlock();
}

void terrainChunk::setLoaded() {
	_loaded = true;
}

bool terrainChunk::loaded() const {
	return _loaded;
}

bool terrainChunk::isValid(const point3Di &nodePos) const
{
	return loaded() && point3Di{nodePos.x/config::chunkSize,nodePos.y/config::chunkSize,nodePos.z/config::chunkSize} == _indexedPosition;
}

rp3d::CollisionBody* terrainChunk::getCollider() const {
	return _collisionBody;
}

bool terrainChunk::operator==(const terrainChunk& right) const {
	return _indexedPosition == right._indexedPosition;
}

bool terrainChunk::operator==(const point3Di& right) const {
	return _indexedPosition == right;
}

bool terrainChunk::operator!=(const terrainChunk& right) const {
	return !(*this == right);
}

bool terrainChunk::operator!=(const point3Di& right) const {
	return _indexedPosition != right;
}

const point3Di& terrainChunk::getPosition() const {
	return _indexedPosition;
}

void terrainChunk::load(const std::filesystem::path& fileName, const point3Di& chunkPos) {
	if (std::filesystem::exists(fileName)) {
		_dirty = true;
		_indexedPosition = chunkPos;
		if (_loaded)
		{
			std::cout << "Loading a new chunk on top of another already loaded one";
		}

		_blocks.clear();
		_blocks.resize(0);

		unsigned blockID;
		blockRotation rotation;
		bool savedMeta;
		unsigned length;

		std::ifstream file(fileName);
		std::string input;
		while (file >> input)
		{
			blockID = std::stoi(input);
			file >> input;
			savedMeta = std::stoi(input);
			if (savedMeta) {
				file >> input;

				rotation = (blockRotation)std::stoi(input);
			}
			file >> input;
			length = std::stoi(input);
			for (int i = 0; i < length; ++i)
			{
				_blocks.push_back({ &baseBlock::terrainTable[blockID],(savedMeta ? rotation : (blockRotation)(rand() % 4)), savedMeta });
			}
		}
		updateAllColliders();
		setLoaded();

	}
}

void terrainChunk::unload(std::filesystem::path file) {
	if (_loaded) {
		_loaded = false;
		if(_dirty)
		{
			file.append(std::to_string(_indexedPosition.x)).append(std::to_string(_indexedPosition.y)).append(std::to_string(_indexedPosition.z)).concat(".z5c");
			if (!std::filesystem::exists(file.parent_path())) {
				std::filesystem::create_directories(file.parent_path());
			}
			std::ofstream outputFile(file);
			//.z5c file format:
			//RLE file which contains the IDs of the blocks on the chunk
			//no header
			//5 continuous blocks of dirt(3) followed by 3 blocks of air(1) would be:
			//3 5
			//1 3
			// STORE TO file
			metaBlock lastBlock = _blocks[0];
			unsigned accumulatedLength = 0;
			for (metaBlock& b : _blocks)
			{
				if (b != lastBlock)
				{
					if (lastBlock.saveMeta) {
						outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << lastBlock.rotation << ' ' << accumulatedLength << std::endl;
					}
					else
					{
						outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << accumulatedLength << std::endl;
					}
					lastBlock = b;
					accumulatedLength = 1;
				}
				else
				{
					accumulatedLength++;
				}
			}
			if (lastBlock.saveMeta) {
				outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << lastBlock.rotation << ' ' << accumulatedLength << std::endl;
			}
			else
			{
				outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << accumulatedLength << std::endl;
			}
		}

		if (_collisionBody != nullptr) {
			Services::physicsMutex.lock();
			{
				Services::collisionWorld->destroyCollisionBody(_collisionBody);
				_collisionBody = nullptr;
			}
			Services::physicsMutex.unlock();
		}
	}
}

void terrainChunk::updateAllColliders()
{
	Services::physicsMutex.lock();
	{
		int counter = 0;
		if (_collisionBody != nullptr) {
			Services::collisionWorld->destroyCollisionBody(_collisionBody);
			_collisionBody = nullptr;
		}
		_collisionBody = Services::collisionWorld->createCollisionBody(rp3d::Transform{ {0,0,0},rp3d::Quaternion::identity() });

		_colliders.clear();
		for (int i = 0; i < config::chunkSize; ++i)
		{
			for (int j = 0; j < config::chunkSize; ++j)
			{
				for (int k = 0; k < config::chunkSize; ++k)
				{
					if (_blocks[counter++].base->solid)
					{
						_colliders.push_back(_collisionBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(i + 0.5),(rp3d::decimal)(j + 0.5),(rp3d::decimal)(k + 0.5)},rp3d::Quaternion::identity() }));
					}
					else
					{
						_colliders.push_back(nullptr);
					}
				}
			}
		}
	}
	Services::physicsMutex.unlock();
}
