#ifndef COMMON_H
#define COMMON_H

#include "glm/glm.hpp"

struct mouse_info {
	bool cur, prev;
	glm::vec2 pos, old_pos;
};

#endif