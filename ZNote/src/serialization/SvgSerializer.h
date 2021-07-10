#pragma once

#include "Scene.h"

namespace app {


	class SvgSerializer {
	public:
		static void Serialize(const char* filepath, const Scene& scene);
	};
}