#pragma once

#include "Event.h"

namespace app {

	class Layer {
	public:
		virtual ~Layer() {}

		virtual void OnAttach() = 0;
		virtual void OnEvent(Event& event) = 0;
		virtual void OnDetach() = 0;

		virtual void OnUpdate() = 0;

		bool CanBeDeleted() const { return m_toDelete; }

	protected:
		bool m_toDelete = false;
	};
}