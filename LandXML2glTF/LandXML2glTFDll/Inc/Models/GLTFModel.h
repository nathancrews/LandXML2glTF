#pragma once
#include "framework.h"
#include <IStreamWriter.h>
#include "Writers/GLTFWriter.h"

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
    unsigned int m_materialId = 0;

    std::vector<float> gltfPolylinePoints;
    std::vector<unsigned int> gltfPolylineIndexIDs;
    std::string accessorIdPolylineIndex;
};

struct GLTFMultiPolyModel
{
    std::string accessorId;
    std::vector<float> gltfMultiPolylinePoints;
    std::vector<GLTFPolylineModel*> gltfPolylines;
};

struct GLTFModel
{
    ~GLTFModel()
    {
        for (GLTFSurfaceModel* gltfSurfModel : gltfSurfaceModels)
        {
            delete gltfSurfModel;
        }

        for (GLTFPolylineModel* gltfPolyModel : gltfMultiPolyModel.gltfPolylines)
        {
            delete gltfPolyModel;
        }
    }

    std::string name;

    std::vector<GLTFSurfaceMaterial> gltfMeshMaterials;
    std::vector<GLTFSurfaceModel*> gltfSurfaceModels;
    GLTFMultiPolyModel gltfMultiPolyModel;

//    std::vector<GLTFPolylineModel*> gltfPolylineModels;
};

}