#pragma once

#include "framework.h"


class LANDXML2GLTFDLLAPI LandXMLModel2GLTF
{
public:

    static bool ConvertFile(const std::string& InLandXMLFilename, const std::string& glTFFilename);

private:

};