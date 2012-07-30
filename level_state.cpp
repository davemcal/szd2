
#include "level_state.h"

#include "graphics2.h"
#include "iterator_2d.h"

#include <vector>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;
using glm::vec2;

const float level_state::MOVE_PER_MS = 0.25f;

//int to string
string itos(int val) {
	stringstream out;
	out << val;
	return out.str();
}

istream& operator>>(istream &stream, level_state::Map& map) {

	stream >> map.width >> map.height;
	
	//Read in tile info
	//0 -> Path, 1+ -> Blocks
	for (int y = 0; y < map.height; y++) {
		for (int x = 0; x < map.width; x++) {
			stream >> map.tiles[x][y];
		}
	}

	//Color info
	for (int y = 0; y < map.height/5; y++) {
		for (int x = 0; x < map.width/5; x++) {
			stream >> map.colors[x][y];
		}
	}

	return stream;
}

istream &operator>>(istream &stream, level_state::Yin &yin) {

	//Starting position
	stream >> yin.tilex >> yin.tiley;
	yin.tilestartx = yin.tilex;
	yin.tilestarty = yin.tiley;

	yin.pixelx = float(yin.tilex * 30);
	yin.pixely = float(yin.tiley * 30);

	return stream;
}

level_state::Yin& level_state::yin_info::operator[](color c) {
	return yins[c];
}

bool level_state::yin_info::on_start(color c) {
	//Use pixel position to account for delay it takes to move one square
	return yins[c].pixelx == yins[c].tilestartx * 30 && yins[c].pixely == yins[c].tilestarty * 30;
}

bool level_state::yin_info::on_end(color c) {
	color other = (c == WHITE) ? BLACK : WHITE;

	//End square is other yin's starting position
	return yins[c].pixelx == yins[other].tilestartx * 30 && yins[c].pixely == yins[other].tilestarty * 30;
}

istream& operator>>(istream &stream, level_state::yin_info& yins) {
	stream >> yins[level_state::WHITE] >> yins[level_state::BLACK];
	
	return stream;
}

level_state::level_state(int _SCREEN_WIDTH, int _SCREEN_HEIGHT, int _level) : 
		level(_level), 
		bonus(_level * BONUS_PER_LEVEL), 
		SCREEN_WIDTH(_SCREEN_WIDTH), 
		SCREEN_HEIGHT(_SCREEN_HEIGHT) {					
	
	string m = "levels\\level";
	m += itos(level);
	m += ".txt";

	ifstream fin(m.c_str());

	if (fin.fail()) throw exception();

	string tileset_name, bgname;
	fin >> tileset_name >> bgname;

	fin >> map;
	
	fin >> player_positions;

	while (!fin.eof()) {
		string message;
		getline(fin, message);

		if (!message.empty()) messages.push_back(message);
	}

	tileset = graphics::get_texture(tileset_name);
	background = graphics::get_texture(bgname);
	border = graphics::get_texture("gfx\\border.png");
	
	yinyangs[WHITE_LEFT] = graphics::get_texture("gfx\\white left.png");
	yinyangs[WHITE_RIGHT] = graphics::get_texture("gfx\\white right.png");
	yinyangs[WHITE_START] = graphics::get_texture("gfx\\white start.png");
	yinyangs[BLACK_LEFT] = graphics::get_texture("gfx\\black left.png");
	yinyangs[BLACK_RIGHT] = graphics::get_texture("gfx\\black right.png");
	yinyangs[BLACK_START] = graphics::get_texture("gfx\\black start.png");
	yinyangs[COMPLETE] = graphics::get_texture("gfx\\complete.png");
	yinyangs[COMPLETE2] = graphics::get_texture("gfx\\complete2.png");
	controls = graphics::get_texture("gfx\\controls.png");

	offset = vec2(float((SCREEN_HEIGHT - map.width * 30) / 2), float((SCREEN_HEIGHT - map.height * 30) / 2));

}

void level_state::display() {

	graphics::draw_tex(vec2( 0.0f, 0.0f ), vec2(float(SCREEN_HEIGHT), float(SCREEN_HEIGHT)), background);
	graphics::draw_tex(vec2( float(SCREEN_HEIGHT), float(SCREEN_HEIGHT - 334) / 2), vec2(256.0f, 334.0f), controls);

	for (int y = 0; y < map.height; y++) {
		for (int x = 0; x < map.width; x++) {
			if (map.tiles[x][y] > 0) {
				//Non-path tile
				graphics::draw_sprite(
					vec2(x * 30, y * 30) + offset, 
					vec2(30, 30), 
					vec2((map.colors[x/5][y/5]*60.0f-60.0f + map.tiles[x][y]*30.0f-30.0f) / 540.0f, (animation_frame / ANIMATION_FRAME_LENGTH + 1) * 30.0f / 120.0f) + vec2(x * 30, y * 30) + offset, 
					vec2(30.0f / 540.0f, 30.0f / 120.0f) + vec2(30, 30), 
					tileset);
			} else {
				//Path-tile
				float bx = 0;
				
				bool left = (x > 0) && !map.tiles[x-1][y];
				bool right = (x < map.width-1) && !map.tiles[x+1][y];
				bool up = (y > 0) && !map.tiles[x][y-1];
				bool down = (y < map.height-1) && !map.tiles[x][y+1];

				bx += (right) ? 0 : 30;
				bx += (down) ? 0 : 60;
				bx += (left) ? 0 : 120;
				bx += (up) ? 0 : 240;

				graphics::draw_sprite(
					vec2(float(x * 30), float(y * 30)) + offset, 
					vec2(30.0f, 30.0f), 
					vec2(bx / 540.0f, 0.0f) + vec2(float(x * 30), float(y * 30)) + offset,
					vec2(30.0f / 540.0f, 30.0f / 120.0f) + vec2(30.0f, 30.0f),
					tileset);
			}
		}
	}

	Yin& pp_white = player_positions[WHITE];
	Yin& pp_black = player_positions[BLACK];

	//Starting positions
	graphics::draw_tex(
		vec2(float(pp_white.tilestartx * 30), float(pp_white.tilestarty * 30)) + offset, 
		vec2(30.0f, 30.0f), 
		yinyangs[WHITE_LEFT]);
	graphics::draw_tex(
		vec2(float(pp_black.tilestartx * 30), float(pp_black.tilestarty * 30)) + offset, 
		vec2(30.0f, 30.0f),
		yinyangs[BLACK_LEFT]);

	//Current positions
	if (player_positions.on_start(WHITE)) 
		graphics::draw_tex(
			vec2(pp_white.tilestartx * 30, pp_white.tilestarty * 30) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[WHITE_START]);
	else if (player_positions.on_end(WHITE)) 
		graphics::draw_tex(
			vec2(pp_black.tilestartx * 30, pp_black.tilestarty * 30) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[COMPLETE]);
	else
		graphics::draw_tex(
			vec2(pp_white.pixelx, pp_white.pixely) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[WHITE_RIGHT]);

	if (player_positions.on_start(BLACK)) 
		graphics::draw_tex(
			vec2(pp_black.tilestartx * 30, pp_black.tilestarty * 30) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[BLACK_START]);
	else if (player_positions.on_end(BLACK)) 
		graphics::draw_tex(
			vec2(pp_white.tilestartx * 30, pp_white.tilestarty * 30) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[COMPLETE2]);
	else
		graphics::draw_tex(
			vec2(pp_black.pixelx, pp_black.pixely) + offset, 
			vec2(30.0f, 30.0f),
			yinyangs[BLACK_RIGHT]);


	//Swap-mode borders
	if (input_state == SWAP) {
		graphics::draw_tex(
			vec2((player_positions[active].tilex / 5) * 150, (player_positions[active].tiley / 5) * 150) + offset,
			vec2(150.0f, 150.0f),
			border);
		graphics::draw_tex(
			vec2(swap_dest.x * 150, swap_dest.y * 150) + offset,
			vec2(150.0f, 150.0f),
			border);
	}

}

void level_state::update(int elapsed, array<bool, SDLK_LAST> prev_input, array<bool, SDLK_LAST> cur_input, mouse_info m) {

	animation_frame = (animation_frame + elapsed) % (ANIMATION_FRAME_LENGTH * 3);

	if (cur_input[SDLK_r]) {
		try {
			change_state(game_state_ptr(new level_state(SCREEN_WIDTH, SCREEN_HEIGHT, level)));
		} catch (exception e) {
			pop_state();
		}
		return;
	}

	switch(input_state) {
	case MOVE:
		update_move(elapsed, prev_input, cur_input);
		break;
	case SWAP:
		update_swap(elapsed, prev_input, cur_input);
		break;
	case VICTORY:
		update_victory(elapsed);
		break;
	default:
		throw exception();
	}
}

template<class InputIterator, class T>
  InputIterator find_first_after ( InputIterator first, InputIterator last, InputIterator initial, const T& value )
  {
	  InputIterator i = initial;

    for (i++ ; i != last; i++) if ( *i == value ) return i;
	for (i = first; i != initial; i++) if (*i == value) return i;
    return last;
  }

void level_state::update_move(int elapsed, array<bool, SDLK_LAST> prev_input, array<bool, SDLK_LAST> cur_input) {

	struct { int x, y; } movement = {0, 0};
	
	if (cur_input[SDLK_TAB] && !prev_input[SDLK_TAB]) active = (active == WHITE) ? BLACK : WHITE;

	Yin& pp_active = player_positions[active];
	Yin& pp_inactive = player_positions[active == WHITE ? BLACK : WHITE];

	if (cur_input[SDLK_SPACE] && !prev_input[SDLK_SPACE] && map.colors[pp_active.tilex / 5][pp_active.tiley / 5] != 1) {
		input_state = SWAP;

		swap_dest.x = player_positions[active].tilex / 5;
		swap_dest.y = player_positions[active].tiley / 5;

		auto it = find_first_after(begin(map.colors), end(map.colors), iterator_at(map.colors, swap_dest.x, swap_dest.y), map.colors[swap_dest.x][swap_dest.y]);

		tie(swap_dest.x, swap_dest.y) = it.loc();
	}

	if (cur_input[SDLK_RIGHT]) { movement.x = 1; movement.y = 0; }
	if (cur_input[SDLK_DOWN]) { movement.x = 0; movement.y = 1; }
	if (cur_input[SDLK_LEFT]) { movement.x = -1; movement.y = 0; }
	if (cur_input[SDLK_UP]) { movement.x = 0; movement.y = -1; }

	if (!pp_active.moving && (movement.x != 0 || movement.y != 0)) {
		pp_active.tilex += movement.x;
		pp_active.tiley += movement.y;
		if (pp_active.tilex < 0 || 
			pp_active.tilex > map.width-1 || 
			pp_active.tiley < 0 || 
			pp_active.tiley > map.height-1 ||
			map.tiles[pp_active.tilex][pp_active.tiley] != 0 ||
			(pp_active.tilex == pp_inactive.tilex && pp_active.tiley == pp_inactive.tiley)) {
				//Can't move this way
				pp_active.tilex -= movement.x;
				pp_active.tiley -= movement.y;
		} else {
			pp_active.moving = true;
		}
	}

	if (pp_active.moving) {
		auto int_compare = [] (int a, int b) -> int { 
			if ( a > b ) return 1;
			if ( a < b) return -1;
			return 0;
		};

		//Direction that the yin needs to head
		movement.x = int_compare(pp_active.tilex * 30, int(pp_active.pixelx));
		movement.y = int_compare(pp_active.tiley * 30, int(pp_active.pixely));

		if (movement.x == 0 && movement.y == 0) {
			//At the end
			pp_active.moving = false;
		} else {
			//Move towards the current location
			pp_active.pixelx = movement.x < 0 ?
				max(pp_active.pixelx + float(movement.x * elapsed) * MOVE_PER_MS, pp_active.tilex * 30.0f) :
				min(pp_active.pixelx + float(movement.x * elapsed) * MOVE_PER_MS, pp_active.tilex * 30.0f);
			
			pp_active.pixely = movement.y < 0 ? 
				max(pp_active.pixely + float(movement.y * elapsed) * MOVE_PER_MS, pp_active.tiley * 30.0f) :
				min(pp_active.pixely + float(movement.y * elapsed) * MOVE_PER_MS, pp_active.tiley * 30.0f);
		}
	}

	if (player_positions.on_end(WHITE) && player_positions.on_end(BLACK))
		try {
			change_state(game_state_ptr(new level_state(SCREEN_WIDTH, SCREEN_HEIGHT, level+1)));
		} catch (exception e) {
			pop_state();
		}
}

void level_state::update_swap(int elapsed, array<bool, SDLK_LAST> prev_input, array<bool, SDLK_LAST> cur_input) {
	if (cur_input[SDLK_SPACE] && !prev_input[SDLK_SPACE]) input_state = MOVE;

	if ((cur_input[SDLK_RIGHT] && !prev_input[SDLK_RIGHT]) || (cur_input[SDLK_DOWN] && !prev_input[SDLK_DOWN])) {
		auto cur = iterator_at(map.colors, swap_dest.x, swap_dest.y);

		auto it = find_first_after(begin(map.colors), end(map.colors), cur, map.colors[swap_dest.x][swap_dest.y]);

		//Don't use the square that you're already in
		if (it == iterator_at(map.colors, player_positions[active].tilex/5, player_positions[active].tiley/5))
			it = find_first_after(begin(map.colors), end(map.colors), it, map.colors[swap_dest.x][swap_dest.y]);

		tie(swap_dest.x, swap_dest.y) = it.loc();
	}

	if ((cur_input[SDLK_LEFT] && !prev_input[SDLK_LEFT]) || (cur_input[SDLK_UP] && !prev_input[SDLK_UP])) {
		auto cur = r_iterator_at(map.colors, swap_dest.x, swap_dest.y);

		auto it = find_first_after(rbegin(map.colors), rend(map.colors), cur, map.colors[swap_dest.x][swap_dest.y]);

		//Don't use the square that you're already in
		if (it == r_iterator_at(map.colors, player_positions[active].tilex/5, player_positions[active].tiley/5))
			it = find_first_after(rbegin(map.colors), rend(map.colors), it, map.colors[swap_dest.x][swap_dest.y]);

		tie(swap_dest.x, swap_dest.y) = (--it.base()).loc();
	}

	if (cur_input[SDLK_RETURN] && !prev_input[SDLK_RETURN]) {

		xy swap_src = { player_positions[active].tilex / 5, player_positions[active].tiley / 5 };

		for (int y = 0; y < 5; y++)
			for (int x = 0; x < 5; x++)
				swap(map.tiles[swap_src.x * 5 + x][swap_src.y * 5 + y], 
					map.tiles[swap_dest.x * 5 + x][swap_dest.y * 5 + y]);

		auto& pp_active = player_positions[active];
		auto& pp_inactive = player_positions[active == WHITE ? BLACK : WHITE];

		auto dest_contains = [&] (int x, int y) -> int {
			return (x / 5 == swap_dest.x && y / 5 == swap_dest.y) ? 1 : 0;
		};

		auto src_contains = [&] (int x, int y) -> int {
			return (x / 5 == swap_src.x && y / 5 == swap_src.y) ? 1 : 0;
		};

		auto move_piece = [&] (int &x, int &y) {
			int move_factor = src_contains(x, y) - dest_contains(x, y);

			x += move_factor * (swap_dest.x * 5 - swap_src.x * 5);
			y += move_factor * (swap_dest.y * 5 - swap_src.y * 5);
		};

		move_piece(pp_active.tilex, pp_active.tiley);
		move_piece(pp_inactive.tilex, pp_inactive.tiley);
		move_piece(pp_active.tilestartx, pp_active.tilestarty);
		move_piece(pp_inactive.tilestartx, pp_inactive.tilestarty);

		pp_active.pixelx = pp_active.tilex * 30;
		pp_active.pixely = pp_active.tiley * 30;
		pp_inactive.pixelx = pp_inactive.tilex * 30;
		pp_inactive.pixely = pp_inactive.tiley * 30;

		swap_dest = swap_src;

		input_state = MOVE;
	}
}

void level_state::update_victory(int elapsed) {

}

level_state::~level_state() {

}
