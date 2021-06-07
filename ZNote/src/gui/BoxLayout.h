#pragma once

#include "GuiComponent.h"

namespace gui {

	/*
	* container which positions components in a single direction.
	* If the children size in the not growing direction is greater than the containers size the component shrinks.
	* Otherwise the container grows in size in the specified direction.
	*/
	class BoxLayout : public GuiComponent {
	public:
		enum class Direction { Y_AXIS, X_AXIS };
		enum class Alignment { CENTER, EDGE };
		enum class ScalingOp { KEEP_ASPECT_RATIO };

		BoxLayout(Direction direction) : m_direction(direction) {}


		void Revalidate() override;

	private:
		Direction m_direction;
		Alignment m_alignment = Alignment::CENTER;
		ScalingOp m_scaling = ScalingOp::KEEP_ASPECT_RATIO;

	};
}