////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Nathan C. Crews IV
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "framework.h"
#include "Models/LandXML.h"
#include <GLTFSDK/Serialize.h>
#include <regex>
#include <tinyxml2.h>

using namespace tinyxml2;

namespace LANDXML2GLTF
{

    class LXParser
    {
    public:

        bool ParseLandXMLHeader(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc);
        bool ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc, std::string& exePath);

        // converts an LandXML RGB color string to RGB float values
        static void LXColor2RGB(const std::string& colorValueStr, float& R, float& G, float& B);

    private:

        bool ParseMaterialTable(XMLElement* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials, std::string& exePath);
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

