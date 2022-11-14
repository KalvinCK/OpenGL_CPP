#pragma once

#include <cmath>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


//
class CameraOrbital
{
private:
	float m_raio;
	float m_phi;
	float m_theta;
	vec3 m_position;
	vec3 m_look;
	const float PI = 3.14159265359;

	bool _disable_fov = false;
	bool active_mouse = false;
public:
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;

	CameraOrbital(float raio, float theta, float phi);

	void Ativar();
	void Forward();
	void Back();
	void UpdateTheta(float delta);
	void UpdatePhi(float delta);
private:
	float Sen(float ang);
	float Cos(float ang);

	vec3 cvt2Cartesiano();
	void Update();
};

inline float CameraOrbital::Sen(float ang)
{
	return sin(ang * PI / 180.f);
}
inline float CameraOrbital::Cos(float ang)
{
	return cos(ang * PI / 180.f);
}
