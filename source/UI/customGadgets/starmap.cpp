#include "UI/customGadgets/starmap.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>

starmap::starmap(point2D pos, point2D size, universeNode* universe, universeNode* parent,std::string s)
{
	_selectable = true;
	_position = pos;
	_size = size;
	_name = s;
	_universe = universe;
	_selected = parent;
	_scale = 1/(_selected->getPosition().magnitude()/(sqrt(pow(_size.x,2)+pow(_size.x,2))));
}

void starmap::draw(point2D offset)
{
	HI2::drawRectangle(offset + _position,_size.x,_size.y,HI2::Color::Black);

	drawNodes(offset);

	if(_hovered){
		HI2::drawLines(calculateOrbit(_hovered,config::orbitDebugMultiplier,10000),HI2::Color::Red);
		drawNodeRing(offset,_hovered,HI2::Color::White);
		drawInfo(offset,_hovered,true);
	}
	else{
		HI2::drawLines(calculateOrbit(_selected,config::orbitDebugMultiplier,10000),HI2::Color::Red);
		drawNodeRing(offset,_selected,flashingCycle>0.5?HI2::Color::Green:HI2::Color::LightGrey);
		drawInfo(offset,_selected,true);
	}
}

void starmap::update(const double& dt)
{
	flashingCycle+=dt;
	if(flashingCycle>1)
		flashingCycle=0;
}

universeNode *starmap::getHoveredNode(const point2D &mouse) const
{
	universeNode* closestNode = nullptr;
	int minDistance = 30;
	for(auto& node : *_universe){
		if(&node == _selected)
			continue;
		int nodeSize = node.getDiameter()*_scale;
		if(nodeSize < 2)
			nodeSize=2;

		point2D nodeScreenPos = translateToDisplayCoord(node.getPosition(),node.getParent()) - _position;
		point2D diff = nodeScreenPos-mouse;
		int dist = sqrt(pow(diff.x,2) + pow(diff.y,2));

		dist-=nodeSize;
		if(dist < 0)
			dist = 0;
		if(dist <= minDistance + 2 && ((closestNode && node.getDiameter() < closestNode->getDiameter()) || dist < minDistance)){
			minDistance = dist;
			closestNode = &node;
		}
	}
	return closestNode;
}

void starmap::drawNodes(const point2D& offset) const
{
	for(auto& node : *_universe){
		fdd relativePosition;
		if(node.getDepth()==0){
			relativePosition = _selected->getLocalPos(node.getPosition(),_universe);
		}
		else{
			relativePosition = _selected->getLocalPos(node.getPosition(),node.getParent());
		}
		relativePosition*=_scale;
		int nodeSize = node.getDiameter()*_scale;
		if(nodeSize < 2)
			nodeSize=2;
		HI2::drawRectangle(translateToDisplayCoord(node.getPosition(),node.getParent()) + offset - point2D{(int)(node.getDiameter()*_scale/2),(int)(node.getDiameter()*_scale/2)},nodeSize,nodeSize,node.getMainColor());
	}
}

void starmap::drawNodeRing(const point2D& offset,universeNode *node, HI2::Color color) const
{
	int nodeSize = node->getDiameter()*_scale;
	if(nodeSize < 2)
		nodeSize=2;
	nodeSize+=6;
	HI2::drawEmptyRectangle(point2D{-3,-3} + translateToDisplayCoord(node->getPosition(),node->getParent()) + offset - point2D{(int)(node->getDiameter()*_scale/2),(int)(node->getDiameter()*_scale/2)},nodeSize,nodeSize,color);
}

void starmap::drawInfo(const point2D &offset, universeNode *node, bool fullInfo) const
{
	HI2::Font font = *Services::fonts.getFont("lemon");
	HI2::drawText(font,node->getName(),offset + _position,30,HI2::Color::White);
	HI2::drawText(font,std::to_string(node->getID()),offset + _position + point2D{0,30},30,HI2::Color::White);
	if(fullInfo){
		HI2::drawText(font,"pos: " + std::to_string(node->getPosition().x) + " " +std::to_string(node->getPosition().y) + " " +std::to_string(node->getPosition().z),offset + _position + point2D{0,60},30,HI2::Color::White);
		HI2::drawText(font,"vel: " + std::to_string(node->getVelocity().x) + " " +std::to_string(node->getVelocity().y) + " " +std::to_string(node->getVelocity().z),offset + _position + point2D{0,90},30,HI2::Color::White);
		HI2::drawText(font,"velMagnitude: " + std::to_string(node->getVelocity().magnitude()),offset + _position + point2D{0,120},30,HI2::Color::White);
	}
}

point2D starmap::translateToDisplayCoord(fdd position, universeNode *parent) const
{
	fdd relativePosition;
	if(parent == nullptr){
		relativePosition = _selected->getLocalPos(position,_universe);
	}
	else{
		relativePosition = _selected->getLocalPos(position,parent);
	}
	relativePosition*=_scale;
	return point2D{(int)(mapOffset.x * _scale), (int)(mapOffset.y * _scale)} + _position + relativePosition.getPoint2D() + (_size/2);
}

std::vector<point2D> starmap::calculateOrbit(universeNode *node, double timeStep, int stepCount) const
{
	std::vector<point2D> points;

	if(node->getParent()){
		points.push_back(translateToDisplayCoord(node->getPosition(),node->getParent()));
		point2D* lastPoint = &points[0];

		fdd position = node->getPosition();
		fdd velocity = node->getVelocity();
		while(points.size()<stepCount){
			position += velocity*timeStep;
			velocity +=	node->getParent()->getGravityAcceleration(position);
			points.push_back(translateToDisplayCoord(position,node->getParent()));
		}
	}
	return points;
}

void starmap::update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt)
{
	if(down[HI2::BUTTON::KEY_MOUSEWHEEL_UP]){
		_scale*=2;
		mapOffset = mapOffset / 2;
	}
	if(down[HI2::BUTTON::KEY_MOUSEWHEEL_DOWN]){
		_scale/=2;
	}

	_hovered = getHoveredNode(mouse);

	//dragging
	if(held[HI2::BUTTON::KEY_LEFTCLICK]){
		if(!wasHolding){
			lastMousePos = mouse;
		}
		{
			mapOffset = mapOffset + point2Dd(point2Dd(mouse - lastMousePos) * (1/_scale));
			lastMousePos = mouse;
		}
	}
	if(down[HI2::BUTTON::KEY_LEFTCLICK] && _hovered){
		_selected = _hovered;
		mapOffset = point2D();
	}

	wasHolding = held[HI2::BUTTON::KEY_LEFTCLICK];
}
