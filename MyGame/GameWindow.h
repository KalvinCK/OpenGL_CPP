#pragma once

#include <iostream>
#include <string>
using namespace std;

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

#include "Timer.h"


enum WindowState { WINDOW_NORMAL, WINDOW_FULLSCREEN };
enum VsyncMODE { VSYNC_OFF, VSYNC_ON };

class Error
{
public:
	static void Test_Entity(bool Entity, const std::string& description, bool close = true);
};


class GameWindow
{
private:
	GLFWcursor* cursor;
	GLFWmonitor* monitor;

public:
	static GLFWwindow* Display;

	string Title;
	ivec2 VersionApiGraphic;

	// window values
	static ivec2 Size;
	static ivec2 PixelsFrameBuffer;
	static ivec2 Position;
	static bool isFocus;

	GameWindow();
	~GameWindow();

	void Create();
	bool Run();
	void SwapBuffers();
	void Close();

	// set details window
	// ----------------------------------------------------------------------------------------
	void SetVsyncMODE(VsyncMODE vsync_mode);
	void SetWindowState(WindowState window_state);

	void SetTitleWindow(const string& title);
	void SetIconGLFW(GLFWimage& icon_img);
	void SetCursorGLFW(GLFWimage& cursor_img);

private:
	// Window CALLBACKS
	// ----------------------------------------------------------------------------------------
	static void windowSizeCALLBACK(GLFWwindow* window, int width, int height);
	static void WindowFramebufferCALLBACK(GLFWwindow* window, int width, int height);
	static void windowPositionCALLBACK(GLFWwindow* window, int xpos, int ypos);
	static void windowFocusCALLBACK(GLFWwindow* window, int focused);

};
inline void GameWindow::Close()
{
	glfwSetWindowShouldClose(Display, GLFW_TRUE);
};
inline void GameWindow::SetVsyncMODE(VsyncMODE vsync_mode)
{
	glfwSwapInterval(vsync_mode);
}
