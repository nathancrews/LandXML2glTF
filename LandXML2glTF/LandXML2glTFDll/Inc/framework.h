#pragma once

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _CRT_FUNCTIONS_REQUIRED 1

#include <windows.h>
#include "targetver.h"

#ifdef LANDXML2GLTFDLLAPI_EXPORT
#define LANDXML2GLTFDLLAPI __declspec(dllexport)
#elif defined(LANDXML2GLTFDLLAPI_IMPORT)
#define LANDXML2GLTFDLLAPI __declspec(dllimport)
#else
#define LANDXML2GLTFDLLAPI
#endif
#else
#   define LANDXML2GLTFDLLAPI
#endif



#include <cstdlib>
#include <iostream>

