#pragma once

#include "LandXML.h"
#include <cmath>
#include <Serialize.h>
#include <regex>
#include "tinyxml2.h"

using namespace tinyxml2;

namespace LANDXML2GLTF
{

    class LXParserHelper
    {
    public:

        void ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLMaterialTable& outlandXMLMaterials, std::vector<LandXMLSurface*>& outlandxmlSurfaces);

    private:

        void ParseMaterialTable(XMLNode* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials);
        void ParseSurface(XMLNode* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface);
        void ParseSurfaceBoundaries(XMLNode* LXSurfaceNode, LandXMLSurface& outLandXMLSurface);

        bool ParsePoint3D(XMLNode* LXPointList, LandXMLPoint3D& OutReturnPoint3D);
        bool ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& OutReturnPointList);
        void SplitPointCData(XMLNode* LXPointList, std::vector<std::string>& OutPointYXZArray);
        bool PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints);
        double PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints);
    };

}

