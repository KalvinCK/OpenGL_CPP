
#include "CameraOrbital.h"

#include "GameWindow.h"
#include "Input.h"

extern GameWindow* window;
extern Input* input;

CameraOrbital::CameraOrbital(float raio, float theta, float phi)
{
	m_raio = raio;
	m_theta = theta;
	m_phi = phi;
	Update();

}
void CameraOrbital::Ativar()
{
	if (input->IsMousePress(MOUSE_BUTTON_2))
		active_mouse = active_mouse ? false : true;

	if (active_mouse)
	{
		// desabilita o cursor
		input->CursorState = CURSOR_HIDDEN;

		_disable_fov = true;
	}
	else
	{
		// Habilita o cursor
		input->CursorState = CURSOR_NORMAL;

		_disable_fov = false;
	}

	if (input->IsKeyDown(KEY_W)) Forward();
	if (input->IsKeyDown(KEY_S)) Back();

	static float lastMousePosx = 0.f;
	float dx;
	static float lastMousePosy = 0.f;
	float dy;

	dx = input->CursorPosition.x - lastMousePosx;
	if (fabs(dx) > 10.f) dx = 10.f;
	lastMousePosx = input->CursorPosition.x;
	UpdateTheta(dx);

	dy = input->CursorPosition.y - lastMousePosy;
	if (fabs(dy) > 10.f) dy = 10.f;
	lastMousePosy = input->CursorPosition.y;
	UpdatePhi(dy);

	ViewMatrix = glm::lookAt(m_position, m_look, vec3(0.0, 1.0, 0.0));
	ProjectionMatrix = glm::perspective(glm::radians(90.0f), (float)(window->Size.x / window->Size.y), 0.01f, 500.0f);
}
void CameraOrbital::Forward()
{
	vec3 dir = m_look;
	vec3 veloc = glm::normalize(dir) * vec3(0.25);
	m_position = m_position + veloc;
	m_raio = sqrt(m_position.x * m_position.x + m_position.y * m_position.y + m_position.z * m_position.z);
}
void CameraOrbital::Back()
{
	vec3 dir = m_look;
	vec3 veloc = glm::normalize(dir) * vec3(-0.25);
	m_position = m_position + veloc;
	m_raio = sqrt(m_position.x * m_position.x + m_position.y * m_position.y + m_position.z * m_position.z);

}
void CameraOrbital::UpdateTheta(float delta)
{
	m_theta -= delta;
	Update();
}
void CameraOrbital::UpdatePhi(float delta)
{
	m_phi -= delta;
	if (m_phi < 1.f) m_phi = 1.f;
	if (m_phi > 150.f) m_phi = 150.f;
	Update();
}

vec3 CameraOrbital::cvt2Cartesiano()
{
	float aux = m_raio * Sen(m_phi);
	float x = aux * Sen(m_theta);
	float y = m_raio * Cos(m_phi);
	float z = aux * Cos(m_theta);
	vec3 result(x, y, z);

	return result;
}
void CameraOrbital::Update()
{
	m_position = cvt2Cartesiano();
	m_look = m_position * (-1.f);
}