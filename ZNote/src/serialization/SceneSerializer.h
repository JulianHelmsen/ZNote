#pragma once

#include "Scene.h"

namespace app {

	class SceneSerializer {
	public:
		static void Serialize(const char* filepath, const Scene& scene);
		static void Deserialize(Scene* scene, const char* filepath);
	};
}