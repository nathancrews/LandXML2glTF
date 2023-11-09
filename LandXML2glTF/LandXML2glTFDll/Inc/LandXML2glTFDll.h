#pragma once

#include "framework.h"


class LANDXML2GLTFDLLAPI LandXMLModel2GLTFDLL
{
public:

    bool ConvertFile(const std::string& InLandXMLFilename, const std::string& glTFFilename);

private:

};