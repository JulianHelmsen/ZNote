#include "Window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

static GLFWwindow* s_window = NULL;
static int mousePressed = 0;
static uint32_t mouseX = 0xFFFFFFFF, mouseY = 0xFFFFFFFF;
static std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> s_dragCallback;
static std::function<void()> s_resizeCallback;

void Window::OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny) {

	button -= 1;
	int w, h;
	glfwGetWindowSize(s_window, &w, &h);
	printf("%d, %d, %d, %d\n", x, y, nx, ny);
	int px = x;
	int py = y;
	int inx = nx;
	int iny = ny;

	if(px >= 0 && py >= 0 && py < h && px < w && inx >= 0 && iny >= 0 && iny < h && inx < w)
		s_dragCallback(x, y, nx, ny, button);
}

void Window::SetResizeCallback(std::function<void()> resizeCallback) {
	s_resizeCallback = resizeCallback;
}

void Window::SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> callback) {
	s_dragCallback = callback;
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
		if (action == GLFW_PRESS) {
			mousePressed = button + 1;
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			mouseX = (uint32_t)x;
			mouseY = (uint32_t)y;
		}else {
			// release
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			uint32_t newMousePosX = (uint32_t) x;
			uint32_t newMousePosY = (uint32_t) y;



			if (newMousePosX != mouseX || newMousePosY != mouseY) {
				// mouse dragged somewhere else
				Window::OnMouseDragged(button, mouseX, mouseY, newMousePosX, newMousePosY);
			}

			mousePressed = 0;
		}
	});

	glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {
		uint32_t newMousePosX = (uint32_t)x;
		uint32_t newMousePosY = (uint32_t)y;

		if (mousePressed) {
			// mouse dragged somewhere else
			Window::OnMouseDragged(mousePressed, mouseX, mouseY, newMousePosX, newMousePosY);
		}

		mouseX = newMousePosX;
		mouseY = newMousePosY;

	});

	glfwSetFramebufferSizeCallback(s_window, [](GLFWwindow* window, int w, int h) -> void {
		s_resizeCallback();
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
