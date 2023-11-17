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

        // converts an LandXML RGB color string to RGB float values
        static void LXColor2RGB(const std::string& colorValueStr, float& R, float& G, float& B);

    private:

        bool ParseMaterialTable(XMLElement* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials);
        bool ParseSurface(XMLElement* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface);
        bool ParseSurfacePoints(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface);
        bool ParseSurfaceFaces(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface);
        bool ParseSurfaceBoundaries(XMLElement* LXSurfaceNode, LandXMLSurface& outLandXMLSurface);

        bool ParsePoint3D(XMLNode* LXPointList, LandXMLPoint3D& OutReturnPoint3D);
        bool ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& OutReturnPointList);
        bool ParseFace(XMLNode* LXFaceList, LandXMLSurfaceFace& outReturnFace);
        void SplitCData(XMLNode* LXPointList, std::vector<std::string>& OutPointYXZArray);
    };

}

