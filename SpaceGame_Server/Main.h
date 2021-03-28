#pragma once

#include"EngineInterface.h"



#ifdef _DEBUG
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "yaml-cppd.lib")
#pragma comment(lib, "gainput-d.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fsbank_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "lua54.lib")
#pragma comment(lib, "steam_api64.lib")
#else
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "Engine.lib")
#pragma comment(lib, "yaml-cpp.lib")
#pragma comment(lib, "gainput.lib")
#pragma comment(lib, "fmod_vc.lib")
#pragma comment(lib, "fsbank_vc.lib")
#pragma comment(lib, "fmodstudio_vc.lib")
#pragma comment(lib, "lua54.lib")
#pragma comment(lib, "steam_api64.lib")
#endif




