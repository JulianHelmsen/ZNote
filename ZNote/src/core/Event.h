#pragma once

#include <stdint.h>
#include <string.h>
#include <core/Logger.h>
#include <assert.h>

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
		EventType& GetEvent() {
			static_assert(sizeof(EventType) < sizeof(m_concreteEvent));
			assert(m_typeId == GenerateTypeId<EventType>());
			return *(EventType*)m_concreteEvent;
		}

		template<typename EventType>
		const EventType& GetEvent() const {
			static_assert(sizeof(EventType) < sizeof(m_concreteEvent));
			assert(m_typeId == GenerateTypeId<EventType>());
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

	};

}