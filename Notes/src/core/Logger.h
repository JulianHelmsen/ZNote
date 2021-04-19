#pragma once

#ifdef DEBUG
	#include <stdio.h>
	#define BREAK() __debugbreak()
	#define LOG(...) printf(__VA_ARGS__)
	#define ASSERT(condition) if(!(condition)) {printf("ASSERT failed in line %d of file \"%s\", func: %s", __LINE__, __FILE__, __FUNCTION__); BREAK();}

#else
	#define BREAK()
	#define LOG(...)
	#define ASSERT(condition)
#endif