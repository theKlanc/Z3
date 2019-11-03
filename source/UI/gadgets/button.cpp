#include "UI/gadgets/button.hpp"

button::button(point2D pos, point2D size, std::string s)
{
	_selectable = true;
	_position = pos;
	_size = size;
	_name = s;
}

void button::draw(point2D offset)
{
	HI2::drawRectangle(_position + offset, _size.x, _size.y, (_pressed ? HI2::Color::Green : HI2::Color::Red));
}

void button::update(const unsigned long long& down, const unsigned long long& up, const unsigned long long& held, const point2D& mouse, const double& dt)
{
	_pressed = (held & HI2::BUTTON::KEY_ACCEPT) || (touched(mouse) && (held & HI2::BUTTON::TOUCH));

	if (isPressing())
		_byMouse = (touched(mouse) && (held & HI2::BUTTON::TOUCH));

	if (_oldPressed && !_pressed)
	{
		if (_byMouse)
		{
			_rising = (touched(mouse) && (up & HI2::BUTTON::TOUCH));
		}
		else
		{
			_rising = (up & HI2::BUTTON::KEY_ACCEPT);
		}
	}
}

void button::update(const double& dt)
{
	_oldPressed = _pressed;
	_pressed = false;
	_rising = false;
}

bool button::isPressed()
{
	return _pressed;
}

bool button::isPressing()
{
	return !_oldPressed && _pressed;
}

bool button::isRising()
{
	return _oldPressed && !_pressed;
}

bool button::isRisingInside()
{
	return _rising;
}
