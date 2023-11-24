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
        bool ParseSurface(XMLElement* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints);
        bool ParseSurfacePoints(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints);
        bool ParseSurfaceFaces(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface);
        bool ParseSurfaceBoundaries(XMLElement* LXSurfaceNode, LandXMLSurface& outLandXMLSurface);

        bool ParseAlignments(XMLElement* LXAlignments, LandXMLModel& outLandXMLMDoc);
        bool ParseParcels(XMLElement* LXParcels, LandXMLModel& outLandXMLMDoc);
        bool ParsePlanFeatures(XMLElement* LXPlanFeatures, LandXMLModel& outLandXMLMDoc);
        bool ParseCgPoints(XMLElement* LXCgPoints, LandXMLModel& outLandXMLMDoc);

        bool ParsePoint(XMLElement* LXPointList, LandXMLPoint3D& OutReturnPoint3D, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints);
        bool ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& OutReturnPointList);
        bool ParsePolyline(XMLElement* LXPolyline, LandXMLPolyline* LXPoly, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints);
        bool ParseCoordGeom(XMLElement* LXCordGeom, std::vector<LandXMLPoint3D>& OutReturnPointList, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints);
        bool ParseFace(XMLNode* LXFaceList, LandXMLSurfaceFace& outReturnFace);
        void SplitCData(XMLNode* LXPointList, std::vector<std::string>& OutPointYXZArray);
    };

}

