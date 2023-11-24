#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include "Models/GLTFModel.h"
#include "Helpers/MathHelper.h"
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