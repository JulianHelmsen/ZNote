#include "Scene.h"
#include <GL/glew.h>

namespace app {

	void Scene::CleanUp(Scene& scene) {
		scene.lineBatch.Clear();
		for (Image& image : scene.images)
			glDeleteTextures(1, &image.textureId);
		scene.images.clear();
	}
}