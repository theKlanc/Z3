#pragma once
#include "HI2.hpp"
namespace utils{
	char getChar(HI2::BUTTON b);
	std::string getString(const std::bitset<HI2::BUTTON_SIZE>& buttons, bool caps, std::string str);
}