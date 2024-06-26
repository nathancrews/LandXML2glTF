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
#include <cstdlib>

#include "Models/LandXML.h"
#include "Models/GLTFModel.h"
#include "Helpers/MathHelper.h"
#include "tinyxml2.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

namespace LANDXML2GLTF
{

class LANDXML2GLTFDLLAPI LandXMLModel2glTF
{
public:

    bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename, const std::string& exePath);
    bool CreateGLTFModel(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    void WriteGLTFFile(Microsoft::glTF::Document& document, GLTFModel& gltfModel, std::filesystem::path& glTFFilename);

private:
    
    void SetGLTFModelSceneOffset(const LandXMLModel& landXMLModel);
    void BuildGLTFMaterialTable(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    bool BuildGLTFSurfaceModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    bool BuildGLTFPolylineModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel);
    GLTFPolylineModel* BuildGLTFPolyline(LandXMLPolyline& LXPoly, double& planeElev);

    void AddGLTFSurfaceMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void AddGLTFSurfaceMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene);

    void AddGLTFPolylineMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::BufferBuilder& bufferBuilder);
    void AddGLTFPolylineMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene);

    double m_unitConversionToWG84 = USFT2M; // no conversion if LandXML linearUnit is meters
    double m_sceneOriginOffsetX = 0.0;
    double m_sceneOriginOffsetY = 0.0;
    double m_sceneOriginOffsetZ = 0.0;

    std::string m_exeDataPath = "";
};

}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif