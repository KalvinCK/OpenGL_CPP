
#include "Camera.h"

#include "GameWindow.h"
#include "Input.h"

extern GameWindow* window;
extern Input* input;

vec3 Camera::position = vec3(0, 0, 5);
vec3 Camera::right = vec3(0.0f);
vec3 Camera::up = vec3(0.0f);
mat4 Camera::ViewMatrix = mat4(1.0f);
mat4 Camera::ProjectionMatrix = mat4(1.0f);

Camera::Camera(vec3 position)
{
	this->position = position;

}


void Camera::UpdateFrame()
{
	if (!window->isFocus)
		return;


	UpdateView();
	UpdatePosition();
	UpdateFov();
}

void Camera::UpdateView()
{
	if (input->IsMousePress(MOUSE_BUTTON_2))
	{
		active_mouse = active_mouse ? false : true;
	}

	if (active_mouse)
	{
		// desabilita o cursor
		input->CursorState = CURSOR_HIDDEN;

		horizontalAngle += mouseSpeed * float(window->Size.x / 2.0 - input->CursorPosition.x);
		verticalAngle += mouseSpeed * float(window->Size.y / 2.0 - input->CursorPosition.y);

		_disable_fov = true;
	}
	else
	{
		// Habilita o cursor
		input->CursorState = CURSOR_NORMAL;

		_disable_fov = false;
	}
}
void Camera::UpdatePosition()
{
	glm::vec3 front;

	front.x = cos(verticalAngle) * sin(horizontalAngle);
	front.y = sin(verticalAngle);
	front.z = cos(verticalAngle) * cos(horizontalAngle);

	right = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0, cos(horizontalAngle - 3.14f / 2.0f));

	up = glm::cross(right, front);

	if (input->IsKeyDown(KEY_W)) position += front * Timer::ElapsedTime * speed;
	if (input->IsKeyDown(KEY_S)) position -= front * Timer::ElapsedTime * speed;
	if (input->IsKeyDown(KEY_D)) position += right * Timer::ElapsedTime * speed;
	if (input->IsKeyDown(KEY_A)) position -= right * Timer::ElapsedTime * speed;

	if (input->IsKeyDown(KEY_SPACE)) position += up * Timer::ElapsedTime * speed;
	if (input->IsKeyDown(KEY_C))	 position -= up * Timer::ElapsedTime * speed;

	float turbo = 10.0f;
	if (input->IsKeyDown(KEY_W) && input->IsKeyDown(KEY_LEFT_SHIFT)) position += front * Timer::ElapsedTime * (speed * turbo);
	if (input->IsKeyDown(KEY_S) && input->IsKeyDown(KEY_LEFT_SHIFT)) position -= front * Timer::ElapsedTime * (speed * turbo);


	ProjectionMatrix = glm::perspective(glm::radians(FoV), window->Size.x / (float)window->Size.y, 0.01f, 1000.0f);

	// Camera matrix
	ViewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::UpdateFov()
{
	if (_disable_fov)
	{
		if (input->MouseScrool == SCROOL_FRONT && FoV >= 21)
			FoV -= 2.5f;
		else if (input->MouseScrool == SCROOL_BACK && FoV <= 89)
			FoV += 2.5f;
	}
}


