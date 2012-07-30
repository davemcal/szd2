#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <map>
#include <string>
#include <vector>

#include "GL/glew.h"
#include "glm/glm.hpp"

/*template <class T>
class poly;*/

class graphics {
	std::map<std::string, GLuint> textures;

	GLuint load_texture(std::string file);
	int width, height;

	static graphics* get_inst(int width = 0, int height = 0, bool full_screen = false);

	void inst_draw_pt(glm::vec2 a, float r, float g, float b);
	void inst_draw_line(glm::vec2, glm::vec2, float r = 1.0f, float g = 1.0f, float b = 1.0f);
	//void inst_draw_poly(poly<float>&, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	void inst_draw_sprite(glm::vec2, glm::vec2, glm::vec2, glm::vec2, GLuint, bool flip = false);
	void inst_draw_tex(glm::vec2, glm::vec2, GLuint tex);
	void inst_draw_shader(glm::vec2, glm::vec2, GLuint shader);
	void inst_translate(glm::vec2 v);
	void inst_display();
	void inst_draw_tex_triangle(const std::vector< glm::vec2 > &pts, const std::vector< glm::vec2 > &tex_pts, GLuint tex);

	GLuint inst_get_texture(std::string file);
	GLuint create_shader(std::string vert, std::string frag);

	graphics(int width = 640, int height = 480, bool full_screen = false);
public:
	static GLuint shader_program;
	static GLuint light_shader;
	static GLuint shadow_shader;
	static GLuint penumbra_shader;

	static void create(int width = 640, int height = 480, bool full_screen = false);

	static void draw_pt(glm::vec2 a, float r, float g, float b);
	static void draw_line(glm::vec2, glm::vec2, float r = 1.0f, float g = 1.0f, float b = 1.0f);
	//static void draw_poly(poly<float>&, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
	static void draw_sprite(glm::vec2, glm::vec2, glm::vec2, glm::vec2, GLuint, bool flip = false);
	static void draw_tex(glm::vec2 pos, glm::vec2 box, GLuint tex);
	static void draw_shader(glm::vec2, glm::vec2, GLuint shader);
	static void draw_tex_triangle(const std::vector< glm::vec2 > &pts, const std::vector< glm::vec2 > &tex_pts, GLuint tex);
	static GLuint empty_texture(int w, int h);

	static void translate(glm::vec2 v);
	static void display();

	static GLuint get_texture(std::string file);
	static int get_width();
	static int get_height();
};


#endif
