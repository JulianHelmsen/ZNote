#pragma once

#include <stdint.h>
#include <functional>
#include "Keycodes.h"
#include "Event.h"
#include <glm/glm.hpp>



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

	static void SetEventCallback(std::function<void(app::Event&)> eventCallback);

	static glm::vec2 NormalizeScreenCoordinates(uint32_t x, uint32_t y);
	static void PostEmptyEvent();


	
private:
	static void OnMouseDragged(MouseButton button, uint32_t x, uint32_t y, uint32_t nx, uint32_t ny);
};