
#include "GameWindow.h"


void Error::Test_Entity(bool Entity, const std::string& description, bool close)
{
	try
	{
		if (Entity)
			return;
		else
			throw std::invalid_argument("Error:: in ");

	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << description << std::endl;
		if (close)
		{
			exit(EXIT_FAILURE);
		}
	}
}

GLFWwindow* GameWindow::Display = nullptr;

ivec2 GameWindow::Size = ivec2(1280, 720);
ivec2 GameWindow::PixelsFrameBuffer = ivec2(1280, 720);
ivec2 GameWindow::Position = ivec2(50, 50);

bool GameWindow::isFocus = true;

int _init = GLFW_FALSE;

GameWindow::GameWindow()
{
	Error::Test_Entity(glfwInit(), "Start GLFW"); _init = GLFW_TRUE;

	Title = "GLFW WINDOW";
	VersionApiGraphic = ivec2(4, 6);
	Size = ivec2(1280, 720);
	PixelsFrameBuffer = ivec2(1280, 720);
	Position = ivec2(50, 50);
	isFocus = true;
	
	cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	monitor = glfwGetPrimaryMonitor();
}
GameWindow::~GameWindow()
{
	glfwDestroyWindow(Display);
	glfwTerminate();
}

void GameWindow::Create()
{
	try
	{
		if (_init == GLFW_FALSE)
			throw std::invalid_argument("Error::GameWindow window is not initialized");

	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	PixelsFrameBuffer = Size;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VersionApiGraphic.x);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VersionApiGraphic.y);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);


	Display = glfwCreateWindow(Size.x, Size.y, Title.c_str(), NULL, NULL);
	Error::Test_Entity(Display, "Init GLFW");

	// CALLBACKS window
	glfwSetWindowSizeCallback(Display, windowSizeCALLBACK);
	glfwSetFramebufferSizeCallback(Display, WindowFramebufferCALLBACK);
	glfwSetWindowPosCallback(Display, windowPositionCALLBACK);
	glfwSetWindowFocusCallback(Display, windowFocusCALLBACK);


	glfwSetWindowPos(Display, Position.x, Position.y);
	glfwMakeContextCurrent(Display);

	glfwPollEvents();
}

bool GameWindow::Run()
{
	return (glfwWindowShouldClose(Display) == GLFW_FALSE) ? true : false;
}

void GameWindow::SwapBuffers()
{

	glfwSwapBuffers(Display);
	glfwPollEvents();

}
void GameWindow::SetWindowState(WindowState window_state)
{
	if (window_state)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(Display, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(Display, NULL, 100, 100, 1600, 900, 0);
	}
}
void GameWindow::SetTitleWindow(const string& title)
{
	glfwSetWindowTitle(Display, title.c_str());
}
void GameWindow::SetIconGLFW(GLFWimage& icon_img)
{
	try
	{
		if (icon_img.pixels == 0)
			throw invalid_argument("GLFW::ERROR... IN ICON!");
		else
			glfwSetWindowIcon(Display, 1, &icon_img);
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << endl;
	}
}
void GameWindow::SetCursorGLFW(GLFWimage& cursor_img)
{
	cursor = glfwCreateCursor(&cursor_img, 0, 0);
	try
	{
		if (cursor == NULL)
			throw invalid_argument("GLFW::ERROR... IN CURSOR!");
		else
			glfwSetCursor(Display, cursor);

	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << endl;
	}
}
// Window CALLBACKS
// ----------------------------------------------------------------------------------------
void GameWindow::windowSizeCALLBACK(GLFWwindow* window, int width, int height)
{
	Size = ivec2(width, height);
}
void GameWindow::WindowFramebufferCALLBACK(GLFWwindow* window, int width, int height)
{
	PixelsFrameBuffer = ivec2(width, height);
}
void GameWindow::windowPositionCALLBACK(GLFWwindow* window, int xpos, int ypos)
{
	Position = vec2(xpos, ypos);
}
void GameWindow::windowFocusCALLBACK(GLFWwindow* window, int focused)
{
	isFocus = focused;
}
