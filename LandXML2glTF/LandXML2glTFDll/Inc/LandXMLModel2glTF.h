#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include "Writers/GLTFWriter.h"
#include "Helpers/MathHelper.h"
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
struct GLTFSurfaceModel
{
    std::string name;

    std::vector<std::string> accessorIdIndices;
    std::vector<std::string> accessorIdPositions;

    std::vector<float> gltfMeshPoints;
    std::vector<UINT> gltfMeshIndices;
    std::unordered_map<UINT, std::vector<UINT>> gltfSubMeshIndices;
};


class LANDXML2GLTFDLLAPI LandXMLModel2glTF
{
public:

    bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename);
    bool CreateGLTFModel(const LandXMLModel& landXMLModel, Microsoft::glTF::Document& glTFDocument, std::vector<GLTFSurfaceModel*>& gltfModels);

    void AddGLTFMeshBuffers(std::vector<GLTFSurfaceModel*>& gltfSurfaceModels, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder, std::vector<std::string>& accessorIdIndices, std::vector<std::string>& accessorIdPositions);
    void AddGLTFMesh(std::vector<GLTFSurfaceModel*>& gltfSurfaceModels, Microsoft::glTF::Document& document, const std::vector<std::string>& accessorIdIndices, const std::vector<std::string>& accessorIdPositions);

    void WriteGLTFFile(Microsoft::glTF::Document& document, std::vector<GLTFSurfaceModel*>& gltfSurfaceModels, std::filesystem::path& glTFFilename);

private:

    double m_unitConversionToWG84 = USFT2M; // no conversion if LandXML linearUnit is meters
    OGRSpatialReference* m_landXMLSpatialRef = nullptr;
    OGRCoordinateTransformation* m_wgsTrans = nullptr;
    tinyxml2::XMLDocument* m_LXDocument = nullptr;
    LandXMLModel m_landXMLModel;

    std::vector<GLTFSurfaceModel> gltfSurfaceModels;
};

}


#ifdef _MSC_VER
#   pragma warning(pop)
#endif