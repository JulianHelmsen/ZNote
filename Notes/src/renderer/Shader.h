#pragma once

#include <stdint.h>
#include <string>

namespace utils {
	uint32_t CreateLineShaderProgram();
	uint32_t CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
}