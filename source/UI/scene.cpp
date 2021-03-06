#include "UI/scene.hpp"
#include <algorithm>

scene::scene(std::string name)
{
	_name = name;
}

std::string_view scene::getName()
{
	return _name;
}

void scene::setName(std::string s)
{
	_name = s;
}

void scene::draw()
{
	for (std::shared_ptr<gadget> g : _gadgets) {
		if (g->isVisible() && g->isRenderable({ 0,0 }, { HI2::getScreenWidth(),HI2::getScreenHeight() }))
			g->draw({ 0,0 });
	}
	if (_selected != nullptr && _selected->isVisible() && _selected->isRenderable({ 0,0 },{HI2::getScreenWidth(),HI2::getScreenHeight()})) {
		_selected->drawOverlay({0,0});
	}
}

void scene::update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, double dt)
{
	for (std::shared_ptr<gadget> g : _gadgets) {
		if (g->isActive()) {
			g->update(dt);
			if (down[HI2::BUTTON::TOUCH] && g->touched(mouse - g->getPosition()) && g->isSelectable())
			{
				_selected = g.get();
			}
		}
	}
	if (_selected != nullptr && _selected->isActive())
	{
		if (down[HI2::BUTTON::RIGHT]) {
			gadget* temp = _selected->getRight();
			if (temp != nullptr && temp->isSelectable())
			{
				_selected = temp;
			}
		}
		else if (down[HI2::BUTTON::UP]) {
			gadget* temp = _selected->getUp();
			if (temp != nullptr && temp->isSelectable())
			{
				_selected = temp;
			}
		}
		else if (down[HI2::BUTTON::LEFT]) {
			gadget* temp = _selected->getLeft();
			if (temp != nullptr && temp->isSelectable())
			{
				_selected = temp;
			}
		}
		else if (down[HI2::BUTTON::DOWN]) {
			gadget* temp = _selected->getDown();
			if (temp != nullptr && temp->isSelectable())
			{
				_selected = temp;
			}
		}
	}



	if (_selected != nullptr && _selected->isActive()) {
		_selected->update(down, up, held, mouse-_selected->getPosition(), dt);
	}

}

void scene::addGadget(std::shared_ptr<gadget> g)
{
	_gadgets.push_back(g);
	if (_selected == nullptr && g->isSelectable()) {
		_selected = g.get();
	}
}

void scene::removeGadget(std::shared_ptr<gadget> g)
{
	if(_selected == g.get())
	{
		_selected = nullptr;
	}
	_gadgets.erase(std::find(_gadgets.begin(),_gadgets.end(),g));
}

void scene::select(std::shared_ptr<gadget> g)
{
	_selected = g.get();
}

void scene::clear()
{
	_selected=nullptr;
	_gadgets.clear();
}
