#pragma once


#include <map>
#include <vector>
#include <string>
using namespace std;

#include <ft2build.h>
#include FT_FREETYPE_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

#include "GameWindow.h"
#include "ShaderProgram.h"

struct Character
{
	unsigned int TextureID;
	ivec2 Size;
	ivec2 Bearing;
	unsigned int Advance;
};

class FreeType
{
private:
	GLuint Vao, Vbo;

	vector<unsigned char> chs;

	map<GLchar, Character> characteres;
	ShaderProgram shaderFonts;
public:
	FreeType(std::string fontPath);
	void RenderFrame(std::string text, vec2 position, float scale, vec4 color);
	void Delete();
};


