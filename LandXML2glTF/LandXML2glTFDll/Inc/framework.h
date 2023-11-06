#pragma once

#ifdef _WIN32
#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#endif

#include <cstdlib>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#   ifdef LANDXML2GLTFDLLAPI_EXPORT
#       define LANDXML2GLTFDLLAPI __declspec(dllexport)
#   elif defined(LANDXML2GLTFDLLAPI_IMPORT)
#       define LANDXML2GLTFDLLAPI __declspec(dllimport)
#   else
#       define LANDXML2GLTFDLLAPI
#   endif
#else
#   define LANDXML2GLTFDLLAPI
#endif