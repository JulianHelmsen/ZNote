#include "CanvasLayer.h"
#include "core/Application.h"
#include "renderer/Renderer2D.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/TextureLoader.h"
#include <optional>
#include "os/Utils.h"
#include "tools/TransformTool.h"
#include "tools/Eraser.h"
#include "tools/Pencil.h"
#include "os/Clipboard.h"


namespace app {


	CanvasLayer::~CanvasLayer() {
	
	}

	void CanvasLayer::OnKeyPress(const KeyPressed& event) {
		if (event.keycode == KEY_E)
		Tool::UseTool(new Eraser);
		else if (event.keycode == KEY_P)
		Tool::UseTool(new Pencil);
		else if (event.keycode == KEY_T)
		Tool::UseTool(new TransformTool);
		else if (event.keycode == KEY_I) {
		// load image and add it to scene
			std::optional<std::string> filepath = os::ShowOpenDialog(NULL);
			if (filepath) {
				Image image;
				image.textureId = utils::TextureLoader::LoadTexture(filepath->c_str(), &image.size);
				image.filepath = *filepath;
				AddImage(image);
			}
		}
	}

	void CanvasLayer::OnEvent(Event& event) {
		if (event.IsOfType<MouseDragged>()) {
			OnDrag(event.Get<MouseDragged>());
			event.Handled();
		}else if (event.IsOfType<KeyPressed>()) {
			KeyPressed& keyPressed = event.Get<KeyPressed>();
			OnKeyPress(event.Get<KeyPressed>());
			Tool::ActiveTool()->OnKeyPress(keyPressed.keycode);

		}else if (event.IsOfType<MouseScrolled>()) {
			OnScroll(event.Get<MouseScrolled>());
			event.Handled();
		}else if (event.IsOfType<MousePressed>() || event.IsOfType<MouseReleased>()) {
			OnButtonStateChanged(event);
			event.Handled();
		}else if (event.IsOfType<os::ClipboardImage>()) {
			os::ClipboardImage& clipboardImage = event.Get<os::ClipboardImage>();

			Image image;
			image.textureId = utils::TextureLoader::LoadTexture(clipboardImage.imageData, clipboardImage.width, clipboardImage.height, clipboardImage.numChannels);
			image.filepath = std::string("(none)");
			image.size.x = (float) clipboardImage.width;
			image.size.y = (float) clipboardImage.height;
			AddImage(image);
		}
	}

	void CanvasLayer::OnDetach() {

	}

	void CanvasLayer::OnAttach() {

	}


	

	void CanvasLayer::OnScroll(const MouseScrolled& event) {
		Scene& scene = Application::GetActiveScene();
		if (event.direction > 0)
			scene.scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(1.1f));
		else
			scene.scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(0.9f));
	}

	void CanvasLayer::OnDrag(const MouseDragged& event) {
		const glm::mat4& viewProjectionMatrix = Application::GetViewProjectionMatrix();
		Scene& scene = Application::GetActiveScene();

		glm::mat4 inverse = glm::inverse(viewProjectionMatrix);
		glm::vec2 normalizedOld = inverse * glm::vec4(Window::NormalizeScreenCoordinates(event.startX, event.startY), 0.0f, 1.0f);
		glm::vec2 normalized = inverse * glm::vec4(Window::NormalizeScreenCoordinates(event.endX, event.endY), 0.0f, 1.0f);

		if (event.button == MouseButton::RIGHT) {
			// move camera
			scene.translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			scene.translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}else if (Tool::ActiveTool() && event.button == MouseButton::LEFT) {
			Tool::ActiveTool()->OnDrag(event.button, normalizedOld, normalized);
		}
	}

	void CanvasLayer::OnUpdate() {
		Scene& scene = Application::GetActiveScene();
		const glm::mat4& viewProjectionMatrix = Application::GetViewProjectionMatrix();



		Renderer2D::Begin(viewProjectionMatrix);
		for (const Image& image : scene.images) {
			Renderer2D::DrawImage(image.textureId, image.centerPos - image.size * 0.5f, image.size);
		}
		Renderer2D::End();

		Renderer2D::Begin(viewProjectionMatrix);
		Renderer2D::DrawBatch(scene.lineBatch);
		Renderer2D::End();

		if (Tool::ActiveTool()->WantsToBeDrawn())
			Tool::ActiveTool()->Draw();

	}

	void CanvasLayer::OnButtonStateChanged(Event& event) {
		// TODO: refactor
		const glm::mat4& viewProjectionMatrix = Application::GetViewProjectionMatrix();
		uint32_t x;
		uint32_t y;
		MouseButton button;
		bool isdown;

		if (event.IsOfType<MousePressed>()) {
			isdown = true;
			MousePressed& e = event.Get<MousePressed>();
			x = e.mouseX;
			y = e.mouseY;
			button = e.button;
		}
		else {
			isdown = false;
			MouseReleased& e = event.Get<MouseReleased>();
			x = e.mouseX;
			y = e.mouseY;
			button = e.button;
		}

		glm::mat4 inverse = glm::inverse(viewProjectionMatrix);
		glm::vec2 mousePos = inverse * glm::vec4(Window::NormalizeScreenCoordinates(x, y), 0.0f, 1.0f);

		if (Tool::ActiveTool())
			Tool::ActiveTool()->OnButtonStateChanged(button, mousePos, isdown);
	}

	void CanvasLayer::AddImage(Image& image) {
		Scene& scene = Application::GetActiveScene();

		float aspectRatio = image.size.x / image.size.y;
		image.centerPos = -glm::vec2(scene.translationMatrix[3]);

		float height = 0.5f / scene.scaleMatrix[0][0];
		float width = aspectRatio * height;
		image.size.x = width;
		image.size.y = height;
		scene.images.push_back(image);
	}
}