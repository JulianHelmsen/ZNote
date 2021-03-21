
#include "Window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>


static GLFWwindow* s_window = NULL;
static int mousePressed = 0;
static uint32_t mouseX = 0xFFFFFFFF, mouseY = 0xFFFFFFFF;




static struct {
	std::function<void()> resize;
	std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> drag;
	std::function<void(int)> scrollWheel;
	std::function<void(MouseButton, uint32_t, uint32_t, bool)> buttonCallback;
	std::function<void(uint32_t, uint32_t)> key;
}s_callbacks;

void Window::OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny) {

	button -= 1;
	int w, h;

	glfwGetWindowSize(s_window, &w, &h);

	int px = x;
	int py = y;
	int inx = nx;
	int iny = ny;

	if(px >= 0 && py >= 0 && py < h && px < w && inx >= 0 && iny >= 0 && iny < h && inx < w)
		s_callbacks.drag(x, y, nx, ny, button);
}

void Window::SetKeyCallback(std::function<void(uint32_t, uint32_t)> key) {
	s_callbacks.key = key;
}

void Window::SetResizeCallback(std::function<void()> resizeCallback) {
	s_callbacks.resize = resizeCallback;
}

void Window::SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int)> callback) {
	s_callbacks.drag = callback;
}

void Window::SetScrollWheelCallback(std::function<void(int)> callback) {
	s_callbacks.scrollWheel = callback;
}


void Window::SetMouseButtonCallback(std::function<void(MouseButton, uint32_t, uint32_t, bool)> callback) {
	s_callbacks.buttonCallback = callback;
}



void* Window::GetNativeHandle() {
	return (void*) s_window;
}

uint32_t Window::GetWidth() {
	int x, y;
	glfwGetWindowSize(s_window, &x, &y);
	return (uint32_t)x;
}

uint32_t Window::GetHeight() {
	int x, y;
	glfwGetWindowSize(s_window, &x, &y);
	return (uint32_t)y;
}


void Window::Create() {
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES, 4);
	s_window = glfwCreateWindow(1020, 720, "Drawing", NULL, NULL);
	glfwMakeContextCurrent(s_window);

	glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) -> void {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		uint32_t cursorX = (uint32_t)x;
		uint32_t cursorY = (uint32_t)y;

		if(s_callbacks.buttonCallback)
			s_callbacks.buttonCallback((MouseButton) button, cursorX, cursorY, action == GLFW_PRESS);

		if (action == GLFW_PRESS)
			mousePressed = button + 1;
		else
			mousePressed = 0;
		

		mouseX = cursorX;
		mouseY = cursorY;
	});

	glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {
		uint32_t newMousePosX = (uint32_t)x;
		uint32_t newMousePosY = (uint32_t)y;

		if (mousePressed && (newMousePosX != mouseX || newMousePosY != mouseY)) {
			// mouse dragged somewhere else
			Window::OnMouseDragged(mousePressed, mouseX, mouseY, newMousePosX, newMousePosY);
		}

		mouseX = newMousePosX;
		mouseY = newMousePosY;

	});

	glfwSetFramebufferSizeCallback(s_window, [](GLFWwindow* window, int w, int h) -> void {
		s_callbacks.resize();
	});

	glfwSetScrollCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {
		int dir = (int) y;
		if(s_callbacks.scrollWheel)
			s_callbacks.scrollWheel(dir);
	});

	glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
		if (action == GLFW_PRESS && s_callbacks.key) {
			s_callbacks.key(key, mods);
		}
	});

	
}


void Window::Destroy() {
	glfwDestroyWindow(s_window);
	glfwTerminate();
}

void Window::PollEvents() {
	glfwWaitEvents();
}

void Window::SwapBuffers() {
	glfwSwapBuffers(s_window);
}

bool Window::IsClosed() {
	return glfwWindowShouldClose(s_window);
}
