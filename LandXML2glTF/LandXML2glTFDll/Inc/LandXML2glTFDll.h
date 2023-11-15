#pragma once

#include "framework.h"
#include <string>

class LANDXML2GLTFDLLAPI LandXMLModel2GLTFDLL
{
public:

    bool ConvertFile(const std::string& InLandXMLFilename, const std::string& glTFFilename, const std::string& inAppPath);

private:

};