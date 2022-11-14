#pragma once

// Include GLEW
#include <GL/glew.h>


class Cube
{
private:
	GLuint cubeVAO;
	GLuint cubeVBO;
public:
	Cube();
	void RenderCube();
	void Delete();

};

