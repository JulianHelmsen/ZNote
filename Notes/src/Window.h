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
	
	static uint32_t GetWidth();
	static uint32_t GetHeight();
	
	static void* GetNativeHandle();


	static void SetDragCallback(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t, int button)> callback);
	static void SetResizeCallback(std::function<void()> resizeCallback);
	static void SetScrollWheelCallback(std::function<void(int)> callback);
	static void SetSaveCallback(std::function<void()> callback);
	static void SetLoadCallback(std::function<void()> callback);
	
private:
	static void OnMouseDragged(int button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny);

	
};