#include "states/state_demo.hpp"
#include <iostream>
#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_mixer.h"
#include "services.hpp"

State::Demo::Demo(gameCore& c) : State_Base(c) {
	pixelSpd.x = 1;
	pixelSpd.y = 1;
	pixelPos.x = 60;
	pixelPos.y = 60;
	texture = Services::graphics.loadTexture("test");
	font = *Services::fonts.loadFont("test");
	effect = HI2::Audio(HI2::getDataPath().append("sounds/sfx/oof.mp3"), false, 1);
	//--


	// Load sound file to use
	// Sound from https://freesound.org/people/jens.enk/sounds/434610/

	//---
}

State::Demo::~Demo() {

}

void State::Demo::input(double dt) {

	const std::bitset<HI2::BUTTON_SIZE>& held = HI2::getKeysHeld();
	if (held[HI2::BUTTON::BUTTON_MINUS]) {
		pixelSpd.x = 0;
		pixelSpd.y = 0;
	}

	if (held[HI2::BUTTON::BUTTON_UP]) {
		pixelSpd.y -= 1;
	}
	if (held[HI2::BUTTON::BUTTON_DOWN]) {
		pixelSpd.y += 1;
	}
	if (held[HI2::BUTTON::BUTTON_LEFT]) {
		pixelSpd.x -= 1;
	}
	if (held[HI2::BUTTON::BUTTON_RIGHT]) {
		pixelSpd.x += 1;
	}
	if (held[HI2::BUTTON::BUTTON_PLUS]) {
		_core->quit();
	}
	if (held[HI2::BUTTON::KEY_A]) {
		done = true;
	}
	if (held[HI2::BUTTON::KEY_B]) {
		HI2::playSound(effect);
		std::cout << "Played Sound" << std::endl;
	}
	if (held[HI2::BUTTON::KEY_ESCAPE]) {
		_core->popState();
	}
}

void State::Demo::update(double dt) {
	pixelPos.x += pixelSpd.x;
	pixelPos.y += pixelSpd.y;
	//bounds checks
	if (pixelPos.x < 0) {
		pixelSpd.x = pixelSpd.x * -1;
		pixelPos.x = pixelPos.x * -1;
	}
	if (pixelPos.y < 0) {
		pixelSpd.y = pixelSpd.y * -1;
		pixelPos.y = pixelPos.y * -1;
	}
	if (pixelPos.x > HI2::getScreenWidth()) {
		pixelSpd.x = pixelSpd.x * -1;
		pixelPos.x = (pixelPos.x - HI2::getScreenWidth()) * -1 + HI2::getScreenWidth();
	}
	if (pixelPos.y > HI2::getScreenHeight()) {
		pixelSpd.y = pixelSpd.y * -1;
		pixelPos.y = (pixelPos.y - HI2::getScreenHeight()) * -1 + HI2::getScreenHeight();
	}

}

void State::Demo::draw(double dt) {
	HI2::startFrame();
	if (texture != nullptr)
		HI2::drawTexture(*texture, 0, 0, 1);

	HI2::drawText(font, "OOF", point2D{ 0,0 }, 40, HI2::Color(255, 0, 0, 255));
	HI2::drawRectangle(pixelPos, 40, 40, HI2::Color(255, 255, 255, 255));



	HI2::endFrame();
}
