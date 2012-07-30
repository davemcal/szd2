#ifndef GAMESTATE
#define GAMESTATE

#include "common.h"
#include "SDL/SDL.h"

#include <memory>
#include <array>
#include <functional>

class game_state {

public:
	typedef std::unique_ptr<game_state> game_state_ptr;

	game_state() {};
	virtual void display() = 0;
	virtual void update(int elapsed, std::array<bool, SDLK_LAST> prev_input, std::array<bool, SDLK_LAST> cur_input, mouse_info m) = 0;
	virtual ~game_state() {}
	
	static std::function<void(game_state_ptr)> change_state;
	static std::function<void(game_state_ptr)> push_state;
	static std::function<void()> pop_state;

};

typedef std::unique_ptr<game_state> game_state_ptr;

#endif
