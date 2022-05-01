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
#include "os/FileUtils.h"


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
				Image image = utils::TextureLoader::LoadTexture(filepath->c_str());
				image.filepath = *filepath;
				AddImage(image);
			}
		}
		else if(event.keycode == KEY_PLUS) {
			Zoom(1.1f);
		}else if(event.keycode == KEY_MINUS) {
			Zoom(0.9f);
		}
	}

	void CanvasLayer::OnEvent(Event& event) {
		if (event.IsOfType<KeyPressed>()) {
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

			Image image = utils::TextureLoader::LoadTexture(clipboardImage.imageData, clipboardImage.width, 
															clipboardImage.height, clipboardImage.numChannels);
			AddImage(image);
		}else if(event.IsOfType<MouseMoved>()) {
			const MouseMoved& mouseMovedEvent = event.Get<MouseMoved>();

			if (m_pressedButton != MouseButton::NONE) {
				// dragged
				OnDrag(m_pressedButton, mouseMovedEvent);
			}else {	
				glm::mat4 inverse = glm::inverse(Application::GetViewProjectionMatrix());
				glm::vec2 world_pos = inverse * glm::vec4(Window::NormalizeScreenCoordinates(mouseMovedEvent.newX, mouseMovedEvent.newY), 0.0f, 1.0f);
				Tool::ActiveTool()->OnMouseMove(world_pos);
			}
		}else if (event.IsOfType<os::FilePasted>()) {
			const os::FilePasted& pasted = event.Get<os::FilePasted>();
			const char* path = pasted.file;

			if (os::FileUtils::DoesFileExtensionMatch(path, ".png.jpg.jpeg")) {
				AddImage(utils::TextureLoader::LoadTexture(path));
			}
		}
	}

	void CanvasLayer::OnDetach() {

	}

	void CanvasLayer::OnAttach() {

	}

	void CanvasLayer::Zoom(float zoom, const glm::vec2& position) {
		// the position should stay on the same spot on the screen
		Scene& scene = Application::GetActiveScene();

		// approach:
		// oldZoom * (position + oldCameraTranslation) = zoom * oldZoom * (position + newCameraTranslation)
		// position + oldCameraTranslation = zoom * (position + newCameraTranslation)
		// 1 / zoom * (position + oldCameraTranslation) = position + newCameraTranslation
		// solution:
		// 1 / zoom * (position + oldCameraTranslation) - position = newCameraTranslation

		glm::vec2 n;
		n.x = 1.0f / zoom * (position.x + scene.translationMatrix[3][0]) - position.x;
		n.y = 1.0f / zoom * (position.y + scene.translationMatrix[3][1]) - position.y;

		scene.translationMatrix[3][0] = n.x;
		scene.translationMatrix[3][1] = n.y;
		Zoom(zoom);
	}

	void CanvasLayer::Zoom(float zoom) {
		Scene& scene = Application::GetActiveScene();
		scene.scaleMatrix *= glm::scale(glm::mat4(1.0f), glm::vec3(zoom));
	}
	

	void CanvasLayer::OnScroll(const MouseScrolled& event) {
		Scene& scene = Application::GetActiveScene();
		glm::mat4 inverse = glm::inverse(Application::GetViewProjectionMatrix());
		glm::vec2 worldMousePos = inverse * glm::vec4(Window::NormalizeScreenCoordinates(event.mouseX, event.mouseY), 0.0f, 1.0f);
		
		Zoom(event.direction > 0 ? 1.1f : 0.9f, worldMousePos);

	}

	void CanvasLayer::OnDrag(const MouseButton& button, const MouseMoved& moved) {

		glm::vec2 normalizedOld = MouseToWorldSpace(moved.oldX, moved.oldY);
		glm::vec2 normalized = MouseToWorldSpace(moved.newX, moved.newY);

		if (button == MouseButton::RIGHT) {
			// move camera
			Scene& scene = Application::GetActiveScene();
			scene.translationMatrix[3][0] -= normalizedOld.x - normalized.x;
			scene.translationMatrix[3][1] -= normalizedOld.y - normalized.y;
		}else if (Tool::ActiveTool() && button == MouseButton::LEFT) {
			Tool::ActiveTool()->OnDrag(button, normalizedOld, normalized);
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
			m_pressedButton = e.button;
		}else {
			isdown = false;
			MouseReleased& e = event.Get<MouseReleased>();
			x = e.mouseX;
			y = e.mouseY;
			button = e.button;
			m_pressedButton = MouseButton::NONE;
		}

		glm::vec2 mousePos = MouseToWorldSpace(x, y);

		if (Tool::ActiveTool())
			Tool::ActiveTool()->OnButtonStateChanged(button, mousePos, isdown);
	}

	glm::vec2 CanvasLayer::MouseToWorldSpace(glm::vec2 mouse_pos) const {
		glm::mat4 inverse = glm::inverse(Application::GetViewProjectionMatrix());
		return inverse * glm::vec4(Window::NormalizeScreenCoordinates(mouse_pos.x, mouse_pos.y), 0.0f, 1.0f);
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