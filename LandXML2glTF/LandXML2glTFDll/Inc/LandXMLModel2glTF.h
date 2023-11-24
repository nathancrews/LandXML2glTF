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
    std::vector<std::string> accessorIdPositions;
    std::vector<std::string> accessorIdIndices;

    std::unordered_map<unsigned int, unsigned int> gltfSubMeshIndicesMaterialMap;
    std::unordered_map<unsigned int, std::vector<unsigned int>> gltfSubMeshIndexIDs;
};

struct GLTFPolylineModel
{
    std::string name;
    unsigned int m_materialId;

    std::vector<float> gltfPolylinePoints;
    std::vector<std::string> accessorIdPolylinePositions;
    std::vector<std::string> accessorIdPolylineIndices;
    std::vector<unsigned int> gltfPolylineIndexIDs;
};

struct GLTFModel
{
    ~GLTFModel()
    {
        for (GLTFSurfaceModel* gltfSurfModel : gltfSurfaceModels)
        {
            delete gltfSurfModel;
        }

        for (GLTFPolylineModel* gltfPolyModel : gltfPolylineModels)
        {
            delete gltfPolyModel;
        }
    }

    std::string name;

    std::vector<GLTFSurfaceMaterial> gltfMeshMaterials;
    std::vector<GLTFSurfaceModel*> gltfSurfaceModels;
    std::vector<GLTFPolylineModel*> gltfPolylineModels;
};


class LANDXML2GLTFDLLAPI LandXMLModel2glTF
{
public:

    bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename);
    bool CreateGLTFModel(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);

    void WriteGLTFFile(Microsoft::glTF::Document& document, GLTFModel& gltfModel, std::filesystem::path& glTFFilename);

private:
    
    void SetGLTFModelSceneOffset(const LandXMLModel& landXMLModel);
    void BuildGLTFMaterialTable(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    bool BuildGLTFSurfaceModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    bool BuildGLTFPolylineModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    GLTFPolylineModel* BuildGLTFPolyline(LandXMLPolyline& LXPoly);

    void AddGLTFSurfaceMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void AddGLTFSurfaceMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene);

    void AddGLTFPolylineMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void AddGLTFPolylineMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene);

    double m_unitConversionToWG84 = USFT2M; // no conversion if LandXML linearUnit is meters
    double m_sceneOriginOffsetX = 0.0;
    double m_sceneOriginOffsetY = 0.0;
    double m_sceneOriginOffsetZ = 0.0;
};

}


#ifdef _MSC_VER
#   pragma warning(pop)
#endif