#ifndef LEVELSTATE
#define LEVELSTATE

#include "game_state.h"

#include "GL/glew.h"
#include "common.h"
#include <vector>
#include <array>
#include <memory>
#include <istream>

class level_state : public game_state {
	static const int BONUS_PER_LEVEL = 1000, ANIMATION_FRAME_LENGTH = 300;
	static const float MOVE_PER_MS;

	enum yinyang_gfx { WHITE_LEFT = 0, WHITE_RIGHT = 1, WHITE_START = 2, BLACK_LEFT = 3, BLACK_RIGHT = 4, BLACK_START = 5, COMPLETE = 6, COMPLETE2 = 7 };

	struct Map {
		std::array<std::array<int, 25>, 25> tiles;
		std::array<std::array<int, 5>, 5> colors;
		int width;
		int height;
	};

	friend std::istream &operator>>(std::istream &stream, Map &map);

	struct Yin {
		int tilex, tiley, tilestartx, tilestarty;
		float pixelx, pixely;
		bool moving;
		int rot, rot_tick;
	};

	friend std::istream &operator>>(std::istream &stream, Yin &yin);

	enum color { WHITE = 0, BLACK = 1 };

	class yin_info { 
		Yin yins[2];


	public:
		Yin& operator[](color c);
		bool on_start(color c);
		bool on_end(color c);

	};

	friend std::istream &operator>>(std::istream &stream, yin_info &yins);
	
	enum INPUT_STATE { MOVE, SWAP, VICTORY };

	int level, bonus, SCREEN_HEIGHT, SCREEN_WIDTH, animation_frame = 0;
	GLuint tileset, background, border, yinyangs[8], controls;
	glm::vec2 offset;

	Map map; 
	struct xy { int x, y; };
	xy swap_dest;

	yin_info player_positions;
	color active = WHITE;

	INPUT_STATE input_state = MOVE;

	std::vector<std::string> messages;

	void update_move(int elapsed, std::array<bool, SDLK_LAST> prev_input, std::array<bool, SDLK_LAST> cur_input);
	void update_swap(int elapsed, std::array<bool, SDLK_LAST> prev_input, std::array<bool, SDLK_LAST> cur_input);
	void update_victory(int elapsed);

public:
	level_state(int SCREEN_WIDTH, int SCREEN_HEIGHT, int _level);
	void display();
	void update(int elapsed, std::array<bool, SDLK_LAST> prev_input, std::array<bool, SDLK_LAST> cur_input, mouse_info m);
	~level_state();
};

#endif
