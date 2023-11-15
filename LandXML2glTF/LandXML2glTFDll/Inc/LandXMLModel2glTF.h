#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include "Writers/GLTFWriter.h"
#include <iostream>
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
        bool CreateGLTFModel(const LandXMLModel& landXMLModel, Microsoft::glTF::Document& glTFDocument);

        void AddGLTFMeshBuffers(Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder, std::string& accessorIdIndices, std::string& accessorIdPositions);
        void AddGLTFMesh(Microsoft::glTF::Document& document, const std::string& accessorIdIndices, const std::string& accessorIdPositions);

        void WriteGLTFFile(Microsoft::glTF::Document& document, std::filesystem::path& glTFFilename);

    private:

        double m_unitConversionToWG84 = 1.0; // no conversion if LandXML linearUnit is meters
        OGRSpatialReference* m_landXMLSpatialRef = nullptr;
        OGRCoordinateTransformation* m_wgsTrans = nullptr;
        tinyxml2::XMLDocument* m_LXDocument = nullptr;
        LandXMLModel m_landXMLModel;

        std::vector<float> gltfMeshPoints;
        std::vector<UINT> gltfMeshIndices;
        std::vector<std::vector<UINT>> gltfSubMeshIndices;
    };

}


#ifdef _MSC_VER
#   pragma warning(pop)
#endif