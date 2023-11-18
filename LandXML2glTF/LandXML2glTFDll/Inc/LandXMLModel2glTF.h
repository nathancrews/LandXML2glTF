#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include "Writers/GLTFWriter.h"
#include "Helpers/MathHelper.h"
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

struct GLTFSurfaceMaterial
{
    GLTFSurfaceMaterial() 
    {
        m_material.id = "0";
        m_material.metallicRoughness.baseColorFactor = Microsoft::glTF::Color4(0.0f, 1.0f, 0.0f, 1.0f);
        m_material.metallicRoughness.metallicFactor = 0.2f;
        m_material.metallicRoughness.roughnessFactor = 0.4f;
        m_material.doubleSided = true;
    }

    Microsoft::glTF::Color4 m_color = Microsoft::glTF::Color4(0.0f, 1.0f, 0.0f, 1.0f);
    Microsoft::glTF::Material m_material;
};

struct GLTFSurfaceModel
{
    std::string name;

    std::vector<float> gltfMeshPoints;
    std::vector<GLTFSurfaceMaterial> gltfSubMeshMaterials;
    std::vector<std::string> accessorIdPositions;
    std::vector<std::string> accessorIdIndices;

    std::unordered_map<unsigned int, unsigned int> gltfSubMeshIndicesMaterialMap;
    std::unordered_map<unsigned int, std::vector<unsigned int>> gltfSubMeshIndexIDs;
};


class LANDXML2GLTFDLLAPI LandXMLModel2glTF
{
public:

    bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename);
    bool CreateGLTFModel(const LandXMLModel& landXMLModel, Microsoft::glTF::Document& glTFDocument, std::vector<GLTFSurfaceModel*>& gltfModels);

    void AddGLTFMeshBuffers(std::vector<GLTFSurfaceModel*>& gltfSurfaceModels, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);

    void AddGLTFMesh(std::vector<GLTFSurfaceModel*>& gltfSurfaceModels, Microsoft::glTF::Document& document);

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