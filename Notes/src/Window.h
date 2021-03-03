#pragma once

#include <stdint.h>
#include <functional>

class Window {
public:
	static void Create();
	static void Destroy();
	static void PollEvents();
	static void SwapBuffers();
	static bool IsClosed();
	static void SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> callback);
	static void SetResizeCallback(std::function<void()> resizeCallback);
	static uint32_t GetWidth();
	static uint32_t GetHeight();

private:
	static void OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny);
};