#pragma once

#if defined(_WIN64)
	#ifdef STARRY_BUILD_DLL
		#define STARRY_API __declspec(dllexport)
	#else
		#define STARRY_API __declspec(dllimport)
	#endif
#elif defined(__APPLE__)
	#ifdef STARRY_BUILD_DLL
		#define STARRY_API __attribute__((visibility("default")))
	#else
		#define STARRY_API __attribute__((visibility("default")))
	#endif
#else
	#error "Starry Render only supports Windows and MacOS 64-bit systems."
#endif
