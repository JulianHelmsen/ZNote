[1mdiff --git a/Notes/src/Application.cpp b/Notes/src/Application.cpp[m
[1mindex 447e118..333ad62 100644[m
[1m--- a/Notes/src/Application.cpp[m
[1m+++ b/Notes/src/Application.cpp[m
[36m@@ -1,5 +1,4 @@[m
 #include "Application.h"[m
[31m-#include "Window.h"[m
 #include <GL/glew.h>[m
 #include "Shader.h"[m
 #include "RenderDefaults.h"[m
[36m@@ -26,6 +25,7 @@[m [mnamespace app {[m
 		Window::SetSaveCallback(std::bind(&Application::Save, this));[m
 		Window::SetLoadCallback(std::bind(&Application::Load, this));[m
 		Window::SetScrollWheelCallback(std::bind(&Application::OnScroll, this, std::placeholders::_1));[m
[32m+[m		[32mWindow::SetMouseButtonCallback(std::bind(&Application::OnMouseButtonStateChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));[m
 		glewInit();[m
 		renderer::SetRenderDefaults();[m
 [m
[36m@@ -45,7 +45,6 @@[m [mnamespace app {[m
 		[m
 		m_program = utils::CreateShaderProgram();[m
 		glUseProgram(m_program);[m
[31m-		m_currentColor = BLUE;[m
 		translationMatrix = glm::mat4(1.0f);[m
 		scaleMatrix = glm::mat4(1.0f);[m
 [m
[36m@@ -53,6 +52,14 @@[m [mnamespace app {[m
 		glUniformMatrix4fv(m_uniformLocationViewProjection, 1, GL_FALSE, &scaleMatrix[0][0]);[m
 [m
 		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);[m
[32m+[m		[32mcolorPallete[0] = WHITE;[m
[32m+[m		[32mcolorPallete[1] = PINK;[m
[32m+[m		[32mcolorPallete[2] = CORAL;[m
[32m+[m		[32mcolorPallete[3] = YELLOW;[m
[32m+[m		[32mcolorPallete[4] = TEAL;[m
[32m+[m		[32mcolorPallete[5] = LIGHT_BLUE;[m
[32m+[m		[32mcolorPallete[6] = BLUE;[m
[32m+[m		[32mcolorPallete[7] = PURPLE;[m
 	}[m
 [m
 	void Application::OnResize() {[m
[36m@@ -66,15 +73,23 @@[m [mnamespace app {[m
 [m
 		if (button == 0) {[m
 			m_hostIndices.push_back(m_hostVertices.size());[m
[31m-			m_hostVertices.push_back({ normalizedOld.x, normalizedOld.y, m_currentColor });[m
[32m+[m			[32mm_hostVertices.push_back({ normalizedOld.x, normalizedOld.y, GetColor() });[m
 			m_hostIndices.push_back(m_hostVertices.size());[m
[31m-			m_hostVertices.push_back({ normalized.x, normalized.y, m_currentColor });[m
[32m+[m			[32mm_hostVertices.push_back({ normalized.x, normalized.y, GetColor() });[m
 		}else if (button == 1) {[m
 			translationMatrix[3][0] -= normalizedOld.x - normalized.x;[m
 			translationMatrix[3][1] -= normalizedOld.y - normalized.y;[m
 		}[m
 	}[m
 [m
[32m+[m	[32mvoid Application::OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown) {[m
[32m+[m		[32mif (button == MouseButton::WHEEL && isdown) {[m
[32m+[m			[32mcolorPalleteIdx++;[m
[32m+[m			[32mif (colorPalleteIdx >= sizeof(colorPallete) / sizeof(Color))[m
[32m+[m				[32mcolorPalleteIdx = 0;[m
[32m+[m		[32m}[m
[32m+[m	[32m}[m
[32m+[m
 [m
 	void Application::Update() {[m
 		// clear screen buffer[m
[1mdiff --git a/Notes/src/Application.h b/Notes/src/Application.h[m
[1mindex e418df5..df099c6 100644[m
[1m--- a/Notes/src/Application.h[m
[1m+++ b/Notes/src/Application.h[m
[36m@@ -4,6 +4,7 @@[m
 #include <vector>[m
 #include "Color.h"[m
 #include <glm/glm.hpp>[m
[32m+[m[32m#include "Window.h"[m
 [m
 namespace app {[m
 [m
[36m@@ -38,15 +39,18 @@[m [mnamespace app {[m
 [m
 		void Update();[m
 		void OnMouseDragged(uint32_t oldX, uint32_t oldY, uint32_t x, uint32_t y, int button);[m
[32m+[m		[32mvoid OnMouseButtonStateChanged(MouseButton button, uint32_t x, uint32_t y, bool isdown);[m
 		void OnResize();[m
 		void OnScroll(int dir);[m
[32m+[m		[32minline Color GetColor() const { return colorPallete[colorPalleteIdx]; }[m
 		[m
 		void Save();[m
 		void Load();[m
 [m
[31m-		Color m_currentColor;[m
[32m+[m		[32muint32_t colorPalleteIdx = 0;[m
 		glm::mat4 viewProjectionMatrix;[m
 		glm::mat4 translationMatrix;[m
 		glm::mat4 scaleMatrix;[m
[32m+[m		[32mColor colorPallete[8];[m
 	};[m
 }[m
\ No newline at end of file[m
[1mdiff --git a/Notes/src/Color.h b/Notes/src/Color.h[m
[1mindex 01018c8..2ba04a5 100644[m
[1m--- a/Notes/src/Color.h[m
[1m+++ b/Notes/src/Color.h[m
[36m@@ -24,4 +24,5 @@[m [mnamespace app {[m
 		};[m
 		uint32_t color;[m
 	};[m
[32m+[m
 }[m
\ No newline at end of file[m
[1mdiff --git a/Notes/src/Window.cpp b/Notes/src/Window.cpp[m
[1mindex b891d63..8277c0f 100644[m
[1m--- a/Notes/src/Window.cpp[m
[1m+++ b/Notes/src/Window.cpp[m
[36m@@ -16,6 +16,7 @@[m [mstatic struct {[m
 	std::function<void(int)> scrollWheel;[m
 	std::function<void()> load;[m
 	std::function<void()> save;[m
[32m+[m	[32mstd::function<void(MouseButton, uint32_t, uint32_t, bool)> buttonCallback;[m
 }s_callbacks;[m
 [m
 void Window::OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny) {[m
[36m@@ -38,7 +39,7 @@[m [mvoid Window::SetResizeCallback(std::function<void()> resizeCallback) {[m
 	s_callbacks.resize = resizeCallback;[m
 }[m
 [m
[31m-void Window::SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> callback) {[m
[32m+[m[32mvoid Window::SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int)> callback) {[m
 	s_callbacks.drag = callback;[m
 }[m
 [m
[36m@@ -54,6 +55,11 @@[m [mvoid Window::SetLoadCallback(std::function<void()> callback) {[m
 	s_callbacks.load = callback;[m
 }[m
 [m
[32m+[m[32mvoid Window::SetMouseButtonCallback(std::function<void(MouseButton, uint32_t, uint32_t, bool)> callback) {[m
[32m+[m	[32ms_callbacks.buttonCallback = callback;[m
[32m+[m[32m}[m
[32m+[m
[32m+[m
 [m
 void* Window::GetNativeHandle() {[m
 	return (void*) s_window;[m
[36m@@ -71,6 +77,7 @@[m [muint32_t Window::GetHeight() {[m
 	return (uint32_t)y;[m
 }[m
 [m
[32m+[m
 void Window::Create() {[m
 	glfwInit();[m
 	glfwWindowHint(GLFW_SAMPLES, 4);[m
[36m@@ -78,28 +85,22 @@[m [mvoid Window::Create() {[m
 	glfwMakeContextCurrent(s_window);[m
 [m
 	glfwSetMouseButtonCallback(s_window, [](GLFWwindow* window, int button, int action, int mods) -> void {[m
[31m-		if (action == GLFW_PRESS) {[m
[31m-			mousePressed = button + 1;[m
[31m-			double x, y;[m
[31m-			glfwGetCursorPos(window, &x, &y);[m
[31m-			mouseX = (uint32_t)x;[m
[31m-			mouseY = (uint32_t)y;[m
[31m-		}else {[m
[31m-			// release[m
[31m-			double x, y;[m
[31m-			glfwGetCursorPos(window, &x, &y);[m
[31m-			uint32_t newMousePosX = (uint32_t) x;[m
[31m-			uint32_t newMousePosY = (uint32_t) y;[m
[31m-[m
[31m-[m
[31m-[m
[31m-			if (newMousePosX != mouseX || newMousePosY != mouseY) {[m
[31m-				// mouse dragged somewhere else[m
[31m-				Window::OnMouseDragged(button, mouseX, mouseY, newMousePosX, newMousePosY);[m
[31m-			}[m
[32m+[m		[32mdouble x, y;[m
[32m+[m		[32mglfwGetCursorPos(window, &x, &y);[m
[32m+[m		[32muint32_t cursorX = (uint32_t)x;[m
[32m+[m		[32muint32_t cursorY = (uint32_t)y;[m
[32m+[m
[32m+[m		[32mif(s_callbacks.buttonCallback)[m
[32m+[m			[32ms_callbacks.buttonCallback((MouseButton) button, cursorX, cursorY, action == GLFW_PRESS);[m
 [m
[32m+[m		[32mif (action == GLFW_PRESS)[m
[32m+[m			[32mmousePressed = button + 1;[m
[32m+[m		[32melse[m
 			mousePressed = 0;[m
[31m-		}[m
[32m+[m[41m		[m
[32m+[m
[32m+[m		[32mmouseX = cursorX;[m
[32m+[m		[32mmouseY = cursorY;[m
 	});[m
 [m
 	glfwSetCursorPosCallback(s_window, [](GLFWwindow* window, double x, double y) -> void {[m
[1mdiff --git a/Notes/src/Window.h b/Notes/src/Window.h[m
[1mindex c3c3a92..8ae8c5a 100644[m
[1m--- a/Notes/src/Window.h[m
[1m+++ b/Notes/src/Window.h[m
[36m@@ -4,7 +4,11 @@[m
 #include <functional>[m
 [m
 [m
[31m-[m
[32m+[m[32menum class MouseButton : uint32_t {[m
[32m+[m	[32mWHEEL = 2,[m
[32m+[m	[32mLEFT = 0,[m
[32m+[m	[32mRight = 1[m
[32m+[m[32m};[m
 [m
 class Window {[m
 public:[m
[36m@@ -25,6 +29,7 @@[m [mpublic:[m
 	static void SetScrollWheelCallback(std::function<void(int)> callback);[m
 	static void SetSaveCallback(std::function<void()> callback);[m
 	static void SetLoadCallback(std::function<void()> callback);[m
[32m+[m	[32mstatic void SetMouseButtonCallback(std::function<void(MouseButton, uint32_t, uint32_t, bool)> callback);[m
 	[m
 private:[m
 	static void OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny);[m
