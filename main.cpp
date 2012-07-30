#include <iostream>
#include <array>
#include "graphics2.h"
#include "common.h"

#include "game_state.h"
#include "level_state.h"

#include "SDL/SDL.h"

using namespace std;

const int SCREEN_WIDTH = 1024; //1024;
const int SCREEN_HEIGHT = 768; //768;
const bool FULL_SCREEN = false;

int main(int argc, char **argv) {
	bool running = true;

	graphics::init(SCREEN_WIDTH, SCREEN_HEIGHT, FULL_SCREEN);

	vector<game_state_ptr> state_stack;

	game_state::push_state = [&] (game_state_ptr state) {
		state_stack.push_back(move(state));
	};

	game_state::change_state = [&] (game_state_ptr state) {
		state_stack.pop_back();
		state_stack.push_back(move(state));
	};

	game_state::pop_state = [&] () {
		state_stack.pop_back();
	};

	state_stack.push_back(game_state_ptr(new level_state(SCREEN_WIDTH, SCREEN_HEIGHT, 1)));

	array<bool, SDLK_LAST> prev_input, cur_input;

	fill(begin(cur_input), end(cur_input), false);

	mouse_info m;

	Uint32 lastTick;
	Uint32 curTick = SDL_GetTicks();
	int fps_tracker = 0;
	int fps = 0;

	while (running && !state_stack.empty()) {
		lastTick = curTick;
		curTick = SDL_GetTicks();

		fps_tracker += curTick - lastTick;
		fps++;
		if (fps_tracker > 1000) {
			cout << fps << endl;
			fps_tracker -= 1000;
			fps = 0;
		}

		copy(begin(cur_input), end(cur_input), begin(prev_input));

		SDL_Event event;

		while( SDL_PollEvent( &event ) ) {

			if( event.type == SDL_QUIT )
			{
				running = false;
			}  
			//Key Releases
			if ( event.type == SDL_KEYUP) {
				cur_input[event.key.keysym.sym] = false;
			}
			//Key Presses
			if ( event.type == SDL_KEYDOWN) {
				cur_input[event.key.keysym.sym] = true;
			}
		}

		int x, y;

		Uint8 buttons = SDL_GetMouseState(&x, &y);
		m.old_pos = m.pos;
		m.pos.x = (float)x;
		m.pos.y = (float)y;
		m.prev = m.cur;
		m.cur = (buttons & SDL_BUTTON_LMASK);

		Uint32 elapsed = (curTick - lastTick) > 50 ? 50 : curTick - lastTick;
		state_stack.back()->update(elapsed, prev_input, cur_input, m);

		if (!state_stack.empty()) state_stack.back()->display();

		graphics::render();

		Uint32 ff = SDL_GetTicks();
		SDL_Delay((1000/60) > (ff - curTick) ? 1000 / 60 - (ff - curTick) : 0);
		
	}

	return 0;
}
