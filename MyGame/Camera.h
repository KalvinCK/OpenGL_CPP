#pragma once

#include <math.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Camera
{
private:
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;

	float speed = 3.0f;
	float mouseSpeed = 0.002f;

	bool active_mouse = false;
	bool _disable_fov = false;
	float FoV = 90.0f;

public:


	static vec3 position;
	static vec3 right;
	static vec3 up;
	static mat4 ViewMatrix;
	static mat4 ProjectionMatrix;

	Camera(vec3 position = vec3(0, 0, 5));
	void UpdateFrame();

private:
	void UpdateView();
	void UpdatePosition();
	void UpdateFov();
};

