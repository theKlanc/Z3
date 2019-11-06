#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "states/state_playing.hpp"
#include "states/state_demo.hpp"
#include "states/state_main_menu.hpp"
#include <memory>
#include <iostream>

void gameCore::startGameLoop() {
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTick  = std::chrono::high_resolution_clock::now();
	
	while (HI2::aptMainLoop() && !states.empty() && !_exit) {
		
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTick = std::chrono::high_resolution_clock::now();
		double microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTick - lastTick).count();
		double msOg = microSeconds;

		
		states.top()->input((double)microSeconds/1000000);

		states.top()->update((double)microSeconds/1000000);

		Services::graphics.stepAnimations((double)microSeconds/1000000);
		states.top()->draw((double)msOg/1000000);


  		lastTick = currentTick;
		processStates();
	}
}

void gameCore::quit() { _exit = true; }

void gameCore::processStates() {
	while (_pop > 0) {
		states.pop();
		_pop--;
	}
	while(!pushStates.empty())
	{
		states.push(std::move(pushStates.top()));
		pushStates.pop();
	}
}

gameCore::gameCore() {
	HI2::systemInit();
	srand(time(NULL));
	_exit = false;
	_pop = 0;
	// HI2::consoleInit();
	//pushState(std::make_unique<State::Playing>(*this,"default",0));
	pushState(std::make_unique<State::MainMenu>(*this));
	//pushState(std::make_unique<State::Demo>(*this));
	processStates();
	
}

gameCore::~gameCore() {
	while (!states.empty())
		states.pop();
	HI2::systemFini();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	pushStates.push(std::move(state));
}

void gameCore::popState(int n) { _pop += n; }
