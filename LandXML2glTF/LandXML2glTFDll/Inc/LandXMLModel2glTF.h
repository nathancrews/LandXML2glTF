#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cassert>
#include <cmath>
#include "GLTF.h"
#include <IStreamWriter.h>
#include "tinyxml2.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

class OGRCoordinateTransformation;
class OGRSpatialReference;

namespace LANDXML2GLTF
{

    class LANDXML2GLTFDLLAPI LandXMLModel2glTF
    {
    public:

        bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename);
        void WriteGLTFFile(std::filesystem::path& glTFFilename);

        // helper function to find LandXML Surface triangle texture materials
        bool PointInPolygon(Microsoft::glTF::Vector3& point, std::vector<Microsoft::glTF::Vector3>& polygonPoints);

    private:

        double m_unitConversionToWG84 = 1.0;
        OGRSpatialReference* m_landXMLSpatialRef = nullptr;
        OGRCoordinateTransformation* m_wgsTrans = nullptr;
        tinyxml2::XMLDocument* m_LXDocument = nullptr;
        LandXMLModel m_landXMLModel;
    };

}


#ifdef _MSC_VER
#   pragma warning(pop)
#endif