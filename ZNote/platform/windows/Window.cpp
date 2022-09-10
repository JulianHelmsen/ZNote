#include "core/Window.h"
#include <Windows.h>
#include <windowsx.h>
#include "core/Logger.h"
#include "os/Clipboard.h"
#include "Keycodes.h"

#define ACTION_PRESS (1)
#define ACTION_RELEASED (2)

#define MOUSE_BUTTON_LEFT (0)
#define MOUSE_BUTTON_RIGHT (1)
#define MOUSE_BUTTON_MIDDLE (2)

#define MOD_BIT_SHIFT (0x0001)
#define MOD_BIT_CONTROL (0x0002)
#define MOD_BIT_ALT (0x0004)
#define MOD_BIT_SUPER (0x0008)
#define MOD_BIT_CAPS_LOCK (0x0010)
#define MOD_BIT_NUM_LOCK (0x0020)

#define WM_EMPTY_MESSAGE     (WM_USER + 0)

static HWND s_hWnd = NULL;
static bool s_closed = false;
static HDC s_HDC = NULL;
static HGLRC s_gl_context;
static MouseButton mousePressed = (MouseButton)0xFFFFFFFF;
static uint32_t mouseX = 0xFFFFFFFF, mouseY = 0xFFFFFFFF;




std::function<void(app::Event&)> s_eventCallback;

static uint32_t TranslateKeycode(uint32_t keycode) {
	MAPKEY(keycode, VK_SPACE, KEY_SPACE);
	MAPKEY(keycode, VK_OEM_COMMA, KEY_COMMA);
	
	MAPKEY(keycode, 0x30, KEY_0);
	MAPKEY(keycode, 0x31, KEY_1);
	MAPKEY(keycode, 0x32, KEY_2);
	MAPKEY(keycode, 0x33, KEY_3);
	MAPKEY(keycode, 0x34, KEY_4);
	MAPKEY(keycode, 0x35, KEY_5);
	MAPKEY(keycode, 0x36, KEY_6);
	MAPKEY(keycode, 0x37, KEY_7);
	MAPKEY(keycode, 0x38, KEY_8);
	MAPKEY(keycode, 0x39, KEY_9);
	MAPKEY(keycode, 0x39, KEY_9);

	MAPKEY(keycode, 0x41, KEY_A);
	MAPKEY(keycode, 0x42, KEY_B);
	MAPKEY(keycode, 0x43, KEY_C);
	MAPKEY(keycode, 0x44, KEY_D);
	MAPKEY(keycode, 0x45, KEY_E);
	MAPKEY(keycode, 0x46, KEY_F);
	MAPKEY(keycode, 0x47, KEY_G);
	MAPKEY(keycode, 0x48, KEY_H);
	MAPKEY(keycode, 0x49, KEY_I);
	MAPKEY(keycode, 0x4A, KEY_J);
	MAPKEY(keycode, 0x4B, KEY_K);
	MAPKEY(keycode, 0x4C, KEY_L);
	MAPKEY(keycode, 0x4D, KEY_M);
	MAPKEY(keycode, 0x4E, KEY_N);
	MAPKEY(keycode, 0x4F, KEY_O);
	MAPKEY(keycode, 0x50, KEY_P);
	MAPKEY(keycode, 0x51, KEY_Q);
	MAPKEY(keycode, 0x52, KEY_R);
	MAPKEY(keycode, 0x53, KEY_S);
	MAPKEY(keycode, 0x54, KEY_T);
	MAPKEY(keycode, 0x55, KEY_U);
	MAPKEY(keycode, 0x56, KEY_V);
	MAPKEY(keycode, 0x57, KEY_W);
	MAPKEY(keycode, 0x58, KEY_X);
	MAPKEY(keycode, 0x59, KEY_Y);
	MAPKEY(keycode, 0x5A, KEY_Z);
	MAPKEY(keycode, VK_CONTROL, KEY_MOD_CONTROL);


	MAPKEY(keycode, VK_LWIN, KEY_LEFT_SUPER);
	MAPKEY(keycode, VK_RWIN, KEY_RIGHT_SUPER);

	LOG("Keycode conversion not implemented");
	BREAK();
	return 0;
}


void Window::SetEventCallback(std::function<void(app::Event&)> eventCallback) {
	s_eventCallback = eventCallback;
}

static void OnButtonChange(uint32_t cursorX, uint32_t cursorY, int button, int action);
static void OnKeyChange(uint32_t key, int action, int mods) {
	if (action == ACTION_PRESS) {
		app::Event event;
		app::KeyPressed keypressed;
		keypressed.keycode = (uint32_t)TranslateKeycode((uint32_t) key);
		keypressed.mods = (uint32_t)mods;
		event.Set(keypressed);

		if (s_eventCallback)
			s_eventCallback(event);

	}

}

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_LBUTTONDOWN) {
		OnButtonChange((uint32_t)GET_X_LPARAM(lParam), (uint32_t)GET_Y_LPARAM(lParam), MOUSE_BUTTON_LEFT, ACTION_PRESS);
	}else if (msg == WM_LBUTTONUP) {
		OnButtonChange((uint32_t)GET_X_LPARAM(lParam), (uint32_t)GET_Y_LPARAM(lParam), MOUSE_BUTTON_LEFT, ACTION_RELEASED);
	}else if (msg == WM_RBUTTONDOWN) {
		OnButtonChange((uint32_t)GET_X_LPARAM(lParam), (uint32_t)GET_Y_LPARAM(lParam), MOUSE_BUTTON_RIGHT, ACTION_PRESS);
	}else if (msg == WM_RBUTTONUP) {
		OnButtonChange((uint32_t)GET_X_LPARAM(lParam), (uint32_t)GET_Y_LPARAM(lParam), MOUSE_BUTTON_RIGHT, ACTION_RELEASED);
	}else if (msg == WM_CLOSE) {
		s_closed = true;
	}else if (msg == WM_MOUSEMOVE) {
		uint32_t newMousePosX = (uint32_t)GET_X_LPARAM(lParam);
		uint32_t newMousePosY = (uint32_t)GET_Y_LPARAM(lParam);

		bool moved = newMousePosX != mouseX || newMousePosY != mouseY;

		if (moved) {
			app::Event event;
			app::MouseMoved moved;
			moved.oldX = mouseX;
			moved.oldY = mouseY;
			moved.newX = newMousePosX;
			moved.newY = newMousePosY;
			event.Set(moved);

			if (s_eventCallback)
				s_eventCallback(event);
		}

		mouseX = newMousePosX;
		mouseY = newMousePosY;
	}else if (msg == WM_SIZE) {
		app::Event event;
		app::WindowResized resized;
		resized.newWidth = (uint32_t)LOWORD(lParam);
		resized.newHeight = (uint32_t)HIWORD(lParam);
		event.Set(resized);

		if (s_eventCallback)
			s_eventCallback(event);
	}else if (msg == WM_MOUSEWHEEL) {
		

		app::Event event;
		app::MouseScrolled scrolled;
		scrolled.direction = (int)GET_WHEEL_DELTA_WPARAM(wParam);
		scrolled.mouseX = (uint32_t) GET_X_LPARAM(lParam);
		scrolled.mouseY = (uint32_t) GET_Y_LPARAM(lParam);


		event.Set(scrolled);
		if (s_eventCallback)
			s_eventCallback(event);
	}else if (msg == WM_KEYDOWN) {
		OnKeyChange((uint32_t) wParam, ACTION_PRESS, 0);
	}else if (msg == WM_DROPFILES) {
		HDROP drop = (HDROP)wParam;
		UINT file_count = DragQueryFileA(drop, 0xFFFFFFFF, NULL, 0);

			
		app::Event event;
		os::FilePasted pasted;

		char filepath_buffer[MAX_PATH + 1];

		if (s_eventCallback)
			for (UINT i = 0; i < file_count; i++) {
				DragQueryFileA(drop, i, filepath_buffer, (UINT)sizeof(filepath_buffer));
				pasted.file = filepath_buffer;
				event.Set(pasted);

				s_eventCallback(event);
			}


		DragFinish(drop);
	}


	return DefWindowProc(hWnd, msg, wParam, lParam);
}

static void OnButtonChange(uint32_t cursorX, uint32_t cursorY, int button, int action) {

	app::Event event;
	if (action == ACTION_PRESS) {
		app::MousePressed pressed;
		pressed.mouseX = cursorX;
		pressed.mouseY = cursorY;
		pressed.button = (MouseButton)(button);
		mousePressed = pressed.button;
		event.Set(pressed);
	}else if(action == ACTION_RELEASED) {
		app::MouseReleased released;
		released.mouseX = cursorX;
		released.mouseY = cursorY;
		released.button = (MouseButton)(button);
		event.Set(released);
		mousePressed = (MouseButton)0xFFFFFFFF;
	}

	if (s_eventCallback)
		s_eventCallback(event);
}


void* Window::GetNativeHandle() {
	return s_hWnd;
}

uint32_t Window::GetWidth() {
	RECT clientRect;
	GetClientRect(s_hWnd, &clientRect);

	return clientRect.right - clientRect.left;
}

uint32_t Window::GetHeight() {
	RECT clientRect;
	GetClientRect(s_hWnd, &clientRect);

	return clientRect.bottom - clientRect.top;
}

static void MakeContextCurrent(HWND hwnd) {
	s_HDC = GetDC(s_hWnd);
	// source: https://www.khronos.org/opengl/wiki/Creating_an_OpenGL_Context_(WGL)
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int choosen = ChoosePixelFormat(s_HDC, &pfd);
	if (choosen == 0) {
		ERR("Abort: Failed to choose pixel format\n");
		exit(-1);
	}
	if (!SetPixelFormat(s_HDC, choosen, &pfd)) {
		ERR("Abort: Failed to set pixel format. Reason from GetLastError(): %d\n", GetLastError());
		exit(-1);
	}

	HGLRC s_gl_context = wglCreateContext(s_HDC);
	if (s_gl_context == NULL) {
		ERR("Failed to create the opengl context. Reason: %d\n", GetLastError());
		exit(-1);
	}

	if (!wglMakeCurrent(s_HDC, s_gl_context)) {
		ERR("Failed to make the context current. Reason: %d\n", GetLastError());
		exit(-1);
	}
}

void Window::Create() {
	WNDCLASSEXW cls = { };
	cls.cbSize = sizeof(cls);
	cls.hInstance = GetModuleHandle(NULL);
	cls.lpszClassName = L"ZNOTE WindowCLass";
	cls.lpfnWndProc = WindowProc;
	cls.style = CS_OWNDC;
	cls.hCursor = LoadCursorA(NULL, (LPCSTR) IDC_ARROW);
	RegisterClassExW(&cls);

	// TODO: glfwWindowHint(GLFW_SAMPLES, 4);
	constexpr uint32_t defaultWidth = 1020, defaultHeight = 720;
	
	DWORD dwExStyle = WS_EX_ACCEPTFILES;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;
	s_hWnd = CreateWindowExW(dwExStyle, cls.lpszClassName, L"ZNote", dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, defaultWidth, defaultHeight, NULL, NULL, cls.hInstance, NULL);
	if (s_hWnd == NULL) {
		ERR("Failed to create the window\n");
		exit(-1);
	}
	DragAcceptFiles(s_hWnd, TRUE);
	ShowWindow(s_hWnd, SW_SHOW);
	MakeContextCurrent(s_hWnd);
#if 0

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
#endif
}


void Window::Destroy() {
	wglMakeCurrent(s_HDC, NULL);
	wglDeleteContext(s_gl_context);
	s_gl_context = NULL;
	DestroyWindow(s_hWnd);
}

void Window::PollEvents() {
	MSG msg = { };
	// PeekMessage()
	GetMessage(&msg, s_hWnd, 0, 0);
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void Window::SwapBuffers() {
	::SwapBuffers(s_HDC);
}

bool Window::IsClosed() {
	return s_closed;
}

glm::vec2 Window::NormalizeScreenCoordinates(uint32_t x, uint32_t y) {
	glm::vec2 pos;
	int w = GetWidth();
	int h = GetHeight();
	pos.x = (float)x / w * 2 - 1;
	pos.y = (float)y / h * 2 - 1;
	pos.y = -pos.y;
	return pos;
}


void Window::PostEmptyEvent() {
	PostMessageA(s_hWnd, WM_EMPTY_MESSAGE, 0, 0);
}