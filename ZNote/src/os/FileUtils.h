#pragma once

namespace os {

	class FileUtils {
	public:
		static const char* GetFileExtension(const char* path);

		/*
		* extension is a string of extensions. Delimeter is a dot.
		* Example:
		* .png.jpg
		* Uppercase and lowercase is ignored
		*/
		static bool DoesFileExtensionMatch(const char* path, const char* extension);
	};
}
