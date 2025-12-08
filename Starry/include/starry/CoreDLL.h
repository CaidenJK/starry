#pragma once

#ifdef _WIN64
	#ifdef STARRY_BUILD_DLL
		#define STARRY_API __declspec(dllexport)
	#else
		#define STARRY_API __declspec(dllimport)
	#endif
#else
	#error "Starry Render only supports Windows 64-bit systems."
#endif
