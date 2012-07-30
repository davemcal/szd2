#include "graphics.h"

#include "SDL.h"
#include "SDL_image.h"

#include "glew.h"
#include "glm/glm.hpp"

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;
using glm::vec2;

GLuint graphics::shader_program;
GLuint graphics::light_shader;
GLuint graphics::shadow_shader;
GLuint graphics::penumbra_shader;

string readfile(string name) {
	string shader_txt;
	ifstream in(name.c_str());
	while ( in.good() ) {
		string s;
		getline(in, s);
		shader_txt += (shader_txt == "") ? s : "\n" + s;
	}
	shader_txt += "\0";
	return shader_txt;
}

graphics::graphics(int w, int h, bool f) {
	//Start SDL
    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_Surface *screen;
	if (f) {
		screen = SDL_SetVideoMode( w, h, 32, SDL_OPENGL|SDL_FULLSCREEN );
	} else {
		screen = SDL_SetVideoMode( w, h, 32, SDL_OPENGL);
	}

	glEnable( GL_TEXTURE_2D );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.9f);
 
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	 
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	 
	glOrtho(0, w, h, 0, -1.0f, 1.0f);
		
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	SDL_WM_SetCaption( "Test", NULL );

	glewInit();
	/*if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader) {
		shader_program = create_shader("character.vert", "character.frag");
		light_shader = create_shader("light.vert", "light.frag");
		shadow_shader = create_shader("light.vert", "shadow.frag");
		penumbra_shader = create_shader("character.vert", "penumbra.frag");
	} else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}*/
}

GLuint graphics::create_shader(string vert, string frag) {
	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	string t = readfile(vert);
	const char *text = t.c_str();
	glShaderSource(vert_shader, 1, &text, NULL);
	glCompileShader(vert_shader);
	t = readfile(frag);
	text = t.c_str();
	glShaderSource(frag_shader, 1, &text, NULL);
	glCompileShader(frag_shader);

	int f;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &f);
	if (f == GL_FALSE) printf("compile error in vertex\n");
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &f);
	if (f == GL_FALSE) printf("compile error in frag\n");

	GLuint program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	return program;
}

void graphics::create(int width, int height, bool full_screen) {
	graphics *g = get_inst(width, height, full_screen);

	g->width = width;
	g->height = height;
}

void graphics::inst_draw_pt(vec2 a, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_LINES);

	glVertex3f(a.x - 5.0f, a.y, 0);
	glVertex3f(a.x + 5.0f, a.y, 0);
	
	glVertex3f(a.x, a.y - 5.0f, 0);
	glVertex3f(a.x, a.y + 5.0f, 0);

	glEnd();
}

void graphics::inst_draw_line(vec2 a, vec2 c, float r, float g, float b) {
	glColor3f(r, g, b);
	glBegin(GL_LINES);

	glVertex3f(a.x, a.y, 0);
	glVertex3f(c.x, c.y, 0);


	glEnd();
}

/*void graphics::inst_draw_poly(polyf& p, float r, float g, float b, float a) {
	glColor4f(r, g, b, a);
	glBegin(GL_TRIANGLE_FAN);

	for (size_t i = 0; i < p.size(); i++) {

		glVertex3f(p.point(i).x, p.point(i).y, 0);

	}

	glEnd();
}*/

void graphics::inst_draw_shader(vec2 pos, vec2 box, GLuint shader) {

	glPushMatrix();

	glTranslatef(pos.x, pos.y, 0.0f);
	glScalef(box.x, box.y, 1.0f);

	glUseProgram(shader);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin( GL_QUADS );

		//Top-left vertex (corner)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );

		//Top-right vertex (corner)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f, 0.0f, 0.0f );

		//Bottom-right vertex (corner)
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, 0.0f );

		//Bottom-left vertex (corner)
		glTexCoord2f( 0.0f, 1.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f );

	glEnd();

	glUseProgram(0);

	glPopMatrix();
}


void graphics::inst_draw_tex(vec2 pos, vec2 box, GLuint tex) {

	glPushMatrix();

	glTranslatef(pos.x, pos.y, 0.0f);
	glScalef(box.x, box.y, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin( GL_QUADS );

		//Top-left vertex (corner)
		glTexCoord2f( 0.0f, 0.0f );
		glVertex3f( 0.0f, 0.0f, 0.0f );

		//Top-right vertex (corner)
		glTexCoord2f( 1.0f, 0.0f );
		glVertex3f( 1.0f, 0.0f, 0.0f );

		//Bottom-right vertex (corner)
		glTexCoord2f( 1.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, 0.0f );

		//Bottom-left vertex (corner)
		glTexCoord2f( 0.0f, 1.0f);
		glVertex3f( 0.0f, 1.0f, 0.0f );

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void graphics::inst_draw_sprite(vec2 screen_pos, vec2 screen_size, 
							vec2 tex_pos, vec2 tex_size,
							GLuint tex, bool flip) {

	glPushMatrix();

	glTranslatef(screen_pos.x, screen_pos.y, 0.0f);
	glScalef(screen_size.x, screen_size.y, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin( GL_QUADS );

		//Top-left vertex (corner)
		glTexCoord2f( (flip) ? tex_pos.x + tex_size.x : tex_pos.x, tex_pos.y );
		glVertex3f( 0.0f, 0.0f, 0.0f );

		//Top-right vertex (corner)
		glTexCoord2f( (flip) ? tex_pos.x : tex_pos.x + tex_size.x, tex_pos.y );
		glVertex3f( 1.0f, 0.0f, 0.0f );

		//Bottom-right vertex (corner)
		glTexCoord2f( (flip) ? tex_pos.x : tex_pos.x + tex_size.x, tex_pos.y + tex_size.y );
		glVertex3f( 1.0f, 1.0f, 0.0f );

		//Bottom-left vertex (corner)
		glTexCoord2f( (flip) ? tex_pos.x + tex_size.x : tex_pos.x, tex_pos.y + tex_size.y);
		glVertex3f( 0.0f, 1.0f, 0.0f );

	glEnd();

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void graphics::inst_translate(vec2 v) {
	glTranslatef(v.x, v.y, 0.0f);
}

void graphics::inst_display() {
	SDL_GL_SwapBuffers();

	glClear( GL_COLOR_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

GLuint graphics::load_texture(string file) {
	if (textures.find(file) != textures.end()) {
		return textures[file];
	}

	SDL_Surface *surface;	// This surface will tell us the details of the image
	GLenum texture_format;
	GLint  nOfColors;
	GLuint texture;
	 
//	if ( (surface = IMG_LoadPNG_RW(SDL_RWFromFile(file.c_str(), "rb"))) ) { 
	if ( surface = IMG_Load(file.c_str()) ) { 	 
		// get the number of channels in the SDL surface
		nOfColors = surface->format->BytesPerPixel;
		if (nOfColors == 4)  {   // contains an alpha channel
			if (surface->format->Rmask == 0x000000ff)
					texture_format = GL_RGBA;
			else
					texture_format = GL_BGRA;
		} else if (nOfColors == 3)  {   // no alpha channel
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		} else {
			printf("incorrect texture format\n");
			return -1;
		}
	        
		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );
	 
		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );
	 
		// Set the texture's stretching properties
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		
 
		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
						  texture_format, GL_UNSIGNED_BYTE, surface->pixels );
		//gluBuild2DMipmaps(	GL_TEXTURE_2D, 4, surface->w, surface->h, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
	} 
	else {
		printf("IMG_Load: %s\n", IMG_GetError());
		return -1;
	}    
	 
	// Free the SDL_Surface only if it was successfully created
	if ( surface ) { 
		SDL_FreeSurface( surface );
	}

	textures[file] = texture;

	return texture;
}

GLuint graphics::inst_get_texture(string file) {

	if (textures.find(file) == textures.end()) {
		return load_texture(file);
	}

	return textures[file];
}

graphics* graphics::get_inst(int width, int height, bool full_screen) {
	static graphics g(width, height, full_screen);

	return &g;
}

void graphics::draw_pt(vec2 a, float r, float g, float b) {
	get_inst()->inst_draw_pt(a, r, g, b);
}

void graphics::draw_line(vec2 x, vec2 y, float r, float g, float b) {
	get_inst()->inst_draw_line(x, y, r, g, b);
}

/*void graphics::draw_poly(poly<float>& p, float r, float g, float b, float a) {
	get_inst()->inst_draw_poly(p, r, g, b, a);
}*/

void graphics::draw_sprite(vec2 a, vec2 b, vec2 c, vec2 d, GLuint e, bool flip) {
	get_inst()->inst_draw_sprite(a, b, c, d, e, flip);
}

void graphics::draw_shader(vec2 a, vec2 b, GLuint shader) {
	get_inst()->inst_draw_shader(a, b, shader);
}

void graphics::draw_tex(vec2 a, vec2 b, GLuint tex) {
	get_inst()->inst_draw_tex(a, b, tex);
}

void graphics::translate(vec2 v) {
	get_inst()->inst_translate(v);
}

void graphics::display() {
	get_inst()->inst_display();
}

GLuint graphics::get_texture(std::string file) {
	return get_inst()->inst_get_texture(file);
}

GLuint graphics::empty_texture(int w, int h)	{
	GLuint txtnumber;						// Texture ID
	unsigned int* data;						// Stored Data

	// Create Storage Space For Texture Data (128x128x4)
	data = (unsigned int*)new GLuint[((w * h)* 4 * sizeof(unsigned int))];
		
	fill(data,data + ((w * h)* 4 * sizeof(unsigned int)), 0);	// Clear Storage Memory

	glGenTextures(1, &txtnumber);					// Create 1 Texture
	glBindTexture(GL_TEXTURE_2D, txtnumber);			// Bind The Texture
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, data);			// Build Texture Using Information In data
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	delete [] data;							// Release data

	glBindTexture(GL_TEXTURE_2D, 0);

	return txtnumber;						// Return The Texture ID
}

int graphics::get_height() {
	return get_inst()->height;
}

int graphics::get_width() {
	return get_inst()->width;
}

void graphics::inst_draw_tex_triangle(const vector<vec2 > &pts, const vector<vec2 > &tex_pts, GLuint tex) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );

	//glColor3f(1.0f, 1.0f, 1.0f);

	glBegin( GL_TRIANGLES );
	
	for (size_t i = 0; i < 3; i++) {
		glTexCoord2f( tex_pts[i].x, tex_pts[i].y );
		glVertex3f( pts[i].x, pts[i].y, 0.0f );
	}

	glEnd();

	glDisable(GL_TEXTURE_2D);
	
}

void graphics::draw_tex_triangle(const std::vector<vec2 > &pts, const std::vector<vec2 > &tex_pts, GLuint tex) {
	get_inst()->inst_draw_tex_triangle(pts, tex_pts, tex);
}
	