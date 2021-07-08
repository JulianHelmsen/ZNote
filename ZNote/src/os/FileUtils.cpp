#include "FileUtils.h"
#include <core/Logger.h>
#include <cstring>
#include <cctype>


namespace os {
	const char* FileUtils::GetFileExtension(const char* path) {
		const char* lastDot = NULL;
		const char* it = path;

		while (*it != '\0') {
			if (*it == '.') {
				lastDot = it;
			}
			it++;
		}

		if (lastDot)
			lastDot++;

		// terminates with dot
		if (*lastDot == '\0')
			return NULL;
		return lastDot;
	}

	const char* SkipToDot(const char* str) {
		while (*str != '.') {
			str++;
			if (*str == '\0')
				return NULL;
		}
		return str;
	}

	bool Equals(const char* dotOrNullTerminated, const char* nullTerminated) {

		while (true) {

			bool isDotOrNullTerminated = *dotOrNullTerminated == '\0' || *dotOrNullTerminated == '.';
			bool isNullTerminated = *nullTerminated == '\0';

			if (isDotOrNullTerminated && isNullTerminated) {
				return true;
			}

			if (toupper(*dotOrNullTerminated) != toupper(*nullTerminated))
				return false;

			dotOrNullTerminated++;
			nullTerminated++;
		}

		return true;
	}
	
	bool FileUtils::DoesFileExtensionMatch(const char* path, const char* extension) {
		ASSERT(path);
		const char* fileExtension = GetFileExtension(path);
		if (!fileExtension)
			return extension == NULL || extension[0] == '\0'; // matches if desired extension is empty and file extension is Null

		while (true) {
			
			extension = SkipToDot(extension);
			

			// every extension has been queried
			if (!extension) {
				return false;
			}

			extension++; // move to character after dot


			// compare two extensions (it and extension)
			if (Equals(extension, fileExtension))
				return true;
		}

		return false;
	}
}
