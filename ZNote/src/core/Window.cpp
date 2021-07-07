
#include "Window.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "os/Clipboard.h"


static GLFWwindow* s_window = NULL;
static MouseButton mousePressed = (MouseButton) 0xFFFFFFFF;
static uint32_t mouseX = 0xFFFFFFFF, mouseY = 0xFFFFFFFF;




std::function<void(app::Event&)> s_eventCallback;



void Window::SetEventCallback(std::function<void(app::Event&)> eventCallback) {
	s_eventCallback = eventCallback;
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
	constexpr uint32_t defaultWidth = 1020, defaultHeight = 720;
	s_window = glfwCreateWindow(defaultWidth, defaultHeight, "ZNote", NULL, NULL);
	glfwMakeContextCurrent(s_window);

	glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) -> void {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		uint32_t cursorX = (uint32_t)x;
		uint32_t cursorY = (uint32_t)y;


		app::Event event;
		if (action == GLFW_PRESS) {
			app::MousePressed pressed;
			pressed.mouseX = cursorX;
			pressed.mouseY = cursorY;
			pressed.button = (MouseButton) (button);
			mousePressed = pressed.button;
			event.Set(pressed);
		}else{
			app::MouseReleased released;
			released.mouseX = cursorX;
			released.mouseY = cursorY;
			released.button = (MouseButton) (button);
			event.Set(released);
			mousePressed = (MouseButton)0xFFFFFFFF;
		}
		
		if(s_eventCallback)
			s_eventCallback(event);
	});

	glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {
		uint32_t newMousePosX = (uint32_t)x;
		uint32_t newMousePosY = (uint32_t)y;

		bool moved = newMousePosX != mouseX || newMousePosY != mouseY;

		if(moved) {
			app::Event event;
			app::MouseMoved moved;
			moved.oldX = mouseX;
			moved.oldY = mouseY;
			moved.newX = newMousePosX;
			moved.newY = newMousePosY;
			event.Set(moved);

			if(s_eventCallback)
				s_eventCallback(event);
		}

		mouseX = newMousePosX;
		mouseY = newMousePosY;

	});

	glfwSetFramebufferSizeCallback(s_window, [](GLFWwindow* window, int w, int h) -> void {
		app::Event event;
		app::WindowResized resized;
		resized.newWidth = (uint32_t)w;
		resized.newHeight = (uint32_t)h;
		event.Set(resized);

		s_eventCallback(event);
	});

	glfwSetScrollCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		app::Event event;
		app::MouseScrolled scrolled;
		scrolled.direction = (int)y;
		scrolled.mouseX = (uint32_t)xpos;
		scrolled.mouseY = (uint32_t)ypos;
		

		event.Set(scrolled);
		if (s_eventCallback)
			s_eventCallback(event);
	});

	glfwSetKeyCallback(s_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
		
		
		if (action == GLFW_PRESS) {
			app::Event event;
			app::KeyPressed keypressed; 
			keypressed.keycode = (uint32_t) key;
			keypressed.mods = (uint32_t) mods;
			event.Set(keypressed);

			if(s_eventCallback)
				s_eventCallback(event);
			
		}

	});

	glfwSetDropCallback(s_window, [](GLFWwindow*, int count, const char** paths) -> void {
		if (!s_eventCallback)
			return;
		app::Event event;
		os::FilePasted pasted;

		for (int i = 0; i < count; i++) {
			pasted.file = paths[i];
			event.Set(pasted);
			s_eventCallback(event);
		}
	});

	app::Event event;
	app::WindowResized resized;
	resized.newWidth = defaultWidth;
	resized.newHeight = defaultHeight;
	event.Set(resized);
	s_eventCallback(event);
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

glm::vec2 Window::NormalizeScreenCoordinates(uint32_t x, uint32_t y) {
	glm::vec2 pos;
	int w, h;
	glfwGetWindowSize(s_window, & w, &h);
	pos.x = (float)x / w * 2 - 1;
	pos.y = (float)y / h * 2 - 1;
	pos.y = -pos.y;
	return pos;
}


void Window::PostEmptyEvent() {
	glfwPostEmptyEvent();
}