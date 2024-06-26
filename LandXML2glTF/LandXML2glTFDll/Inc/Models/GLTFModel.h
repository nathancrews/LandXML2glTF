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
#include <GLTFSDK/IStreamWriter.h>
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
};

}