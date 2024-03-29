#pragma once

#include <stdint.h>
#include <string.h>
#include <core/Logger.h>
#include "Keycodes.h"

namespace app {

	
	class Event {
	public:

		template<typename T>
		static constexpr size_t GenerateTypeId() {
			// function signature depends on template argument. Therefore a call with the same function signature 
			// will generate the same type id
			return (size_t) __FUNCSIG__;
		}

		template<typename ConcreteEvent>
		void Set(const ConcreteEvent& event) {
			static_assert(sizeof(event) < sizeof(m_concreteEvent));
			
			memcpy(&m_concreteEvent, &event, sizeof(event));
			m_typeId = GenerateTypeId<ConcreteEvent>();
		}

		template<typename EventType>
		bool IsOfType() {
			return GenerateTypeId<EventType>() == m_typeId;
		}

		template<typename EventType>
		EventType& Get() {
			static_assert(sizeof(EventType) < sizeof(m_concreteEvent));
			ASSERT(m_typeId == GenerateTypeId<EventType>());
			return *(EventType*)m_concreteEvent;
		}

		template<typename EventType>
		const EventType& Get() const {
			static_assert(sizeof(EventType) < sizeof(m_concreteEvent));
			ASSERT(m_typeId == GenerateTypeId<EventType>());
			return *(EventType*)m_concreteEvent;
		}

		inline void Handled() { m_handled = true; }
		inline bool IsHandled() const { return m_handled; }

	private:

		size_t m_typeId;
		char m_concreteEvent[50];
		bool m_handled = false;
	};


	struct MouseMoved {
		uint32_t oldX;
		uint32_t oldY;
		uint32_t newX;
		uint32_t newY;
	};

	struct MousePressed {
		uint32_t mouseX;
		uint32_t mouseY;
		MouseButton button;
	};


	struct MouseReleased {
		uint32_t mouseX;
		uint32_t mouseY;
		MouseButton button;
	};

	struct MouseScrolled {
		uint32_t mouseX;
		uint32_t mouseY;
		int direction;
	};

	struct KeyPressed {
		uint32_t keycode;
		uint32_t mods;
	};

	struct WindowResized {
		uint32_t newWidth;
		uint32_t newHeight;
	};


	struct TouchEvent {
		enum TouchEventType : uint32_t {
			TOUCH_MOTION = 1, TOUCH_PRESS = 2, TOUCH_RELEASE = 4
		};


		enum InputSource {
			FINGER, PEN, PALM
		};

		uint32_t type;
		InputSource source;
		uint32_t touch_id;

		uint32_t x;
		uint32_t y;
		bool is_contact_area_valid;
		float contact_area;
	};




}