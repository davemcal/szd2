#ifndef GRAPHICS2_H
#define GRAPHICS2_H

#include "glm/glm.hpp"
#include "GL/glew.h"

#include <string>

namespace graphics {
	void init(int width = 640, int height = 480, bool full_screen = false);

	GLuint get_texture(std::string file);

	void draw_tex(glm::vec2 pos, glm::vec2 box, GLuint tex, float depth = 0);
	void draw_sprite(glm::vec2 screen_TL, glm::vec2 screen_BR, 
			glm::vec2 tex_TL, glm::vec2 tex_BR,
			GLuint tex, float depth = 0);
	void render();
}

#endif
