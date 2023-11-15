#pragma once

#include "Models/LandXML.h"
#include <cmath>
#include <Serialize.h>
#include <regex>
#include "tinyxml2.h"

using namespace tinyxml2;

namespace LANDXML2GLTF
{

    class LXParser
    {
    public:

        bool ParseLandXMLHeader(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc);
        bool ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc);

    private:

        bool ParseMaterialTable(XMLNode* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials);
        bool ParseSurface(XMLNode* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface);
        bool ParseSurfaceBoundaries(XMLNode* LXSurfaceNode, LandXMLSurface& outLandXMLSurface);

        bool ParsePoint3D(XMLNode* LXPointList, LandXMLPoint3D& OutReturnPoint3D);
        bool ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& OutReturnPointList);
        bool ParseFace(XMLNode* LXFaceList, LandXMLSurfaceFace& outReturnFace);
        void SplitCData(XMLNode* LXPointList, std::vector<std::string>& OutPointYXZArray);
    };

}

