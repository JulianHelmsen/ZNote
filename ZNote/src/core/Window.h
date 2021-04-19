#pragma once

#include <stdint.h>
#include <functional>
#include "Keycodes.h"



class Window {
public:
	/**
	* Should be called after added event listeners
	*/
	static void Create();
	static void Destroy();
	static void PollEvents();
	static void SwapBuffers();
	static bool IsClosed();
	
	static uint32_t GetWidth();
	static uint32_t GetHeight();
	
	static void* GetNativeHandle();


	static void SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> callback);
	static void SetResizeCallback(std::function<void()> resizeCallback);
	static void SetScrollWheelCallback(std::function<void(int)> callback);
	static void SetMouseButtonCallback(std::function<void(MouseButton, uint32_t, uint32_t, bool)> callback);
	static void SetKeyCallback(std::function<void(uint32_t, uint32_t)> key);
	
private:
	static void OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny);

	
};