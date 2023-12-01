#include "LandXMLModel2glTF.h"
#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"
#include "Writers/GLTFWriter.h"
#include <sstream>
#include <stdio.h>

namespace LANDXML2GLTF
{

bool LandXMLModel2glTF::Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename,const std::string& exeDataPath)
{
    bool retval = false;
    LandXMLModel landXMLModel;
    LXParser LXHelper;
    tinyxml2::XMLDocument* LXDocument = nullptr;

    m_exeDataPath = exeDataPath;

#ifdef USE_GDAL
    OGRSpatialReference* landXMLSpatialRef = nullptr;
    OGRCoordinateTransformation* wgsTrans = nullptr;
#endif

    if (false == std::filesystem::exists(InLandXMLFilename))
    {
        std::cout << "Error: failed to find file: " << InLandXMLFilename << "\n";
        return retval;
    }

    LXDocument = new tinyxml2::XMLDocument(true, COLLAPSE_WHITESPACE);

    if (!LXDocument)
    {
        std::cout << "Error: failed to load file: " << InLandXMLFilename << "\n";
        return retval;
    }

    LXDocument->LoadFile(InLandXMLFilename.c_str());


    if (!LXHelper.ParseLandXMLHeader(LXDocument, landXMLModel))
    {
        std::cout << "Error: failed to parse LandXML data from file: " << InLandXMLFilename << "\n";
        return retval;
    }

    if (landXMLModel.m_units.m_linearUnitString.compare("USSurveyFoot"))
    {
        m_unitConversionToWG84 = USFT2M;
    }
    else if (landXMLModel.m_units.m_linearUnitString.compare("foot"))
    {
        m_unitConversionToWG84 = IFT2M;
    }

#ifdef USE_GDAL
    // TODO: NOT in use yet
    //landXMLSpatialRef = new OGRSpatialReference();

    //OGRErr tStat = landXMLSpatialRef->importFromWkt(landXMLModel.m_wktString.c_str());

    //if (tStat == OGRERR_NONE)
    //{
    //    wgsTrans = MathHelper::GetWGS84CoordTransform(*landXMLSpatialRef);
    //    char* units = nullptr;
    //    m_unitConversionToWG84 = landXMLSpatialRef->GetLinearUnits(&units);
    //}
#endif

    std::cout << "Parsing and building LandXML model...\n";

    if (!LXHelper.ParseLandXMLFile(LXDocument, landXMLModel, m_exeDataPath))
    {
        std::cout << "Error: failed to load and parse valid LandXML data from file: " << InLandXMLFilename << "\n";
        return retval;
    }

    Microsoft::glTF::Document glTFDoc;
    GLTFModel gltfModel;

    glTFDoc.asset.generator = "LandXML to glTF 2.0 Converter, version 1.0";
    glTFDoc.asset.copyright = "Nathan Crews";
    std::cout << "Building glTF model...\n";

    try {
        retval = CreateGLTFModel(landXMLModel, gltfModel);

        if (gltfModel.gltfSurfaceModels.size() > 0 || gltfModel.gltfMultiPolyModel.gltfPolylines.size() > 0)
        {
            std::cout << "Writing glTF file: " << glTFFilename << "\n";
            std::filesystem::path glTFFilenameAsPath = glTFFilename;
            WriteGLTFFile(glTFDoc, gltfModel, glTFFilenameAsPath);
        }
        else
        {
            std::cout << "Error: No surfaces or polyline data found in " << InLandXMLFilename << "\n";
        }
    }
    catch (...)
    {
        std::cout << "Critical Error occurred.\n";
    }

    // cleanup memory
    delete LXDocument;

    return retval;
}

bool LandXMLModel2glTF::CreateGLTFModel(const LandXMLModel& landXMLModel, GLTFModel& gltfModel)
{
    bool retVal = false;
    bool surfSuccess = false, polySuccess = false;

    SetGLTFModelSceneOffset(landXMLModel);
    BuildGLTFMaterialTable(landXMLModel, gltfModel);
    surfSuccess = BuildGLTFSurfaceModels(landXMLModel, gltfModel);
    polySuccess = BuildGLTFPolylineModels(landXMLModel, gltfModel);

    retVal = surfSuccess | polySuccess;

    return retVal;
}

void LandXMLModel2glTF::SetGLTFModelSceneOffset(const LandXMLModel& landXMLModel)
{
    // Set the scene origin offset value
    if (landXMLModel.m_landxmlSurfaces.size() > 0)
    {
        m_sceneOriginOffsetX = landXMLModel.m_landxmlSurfaces[0]->m_minSurfPoint.x;
        m_sceneOriginOffsetY = landXMLModel.m_landxmlSurfaces[0]->m_minSurfPoint.y;
        m_sceneOriginOffsetZ = landXMLModel.m_landxmlSurfaces[0]->m_minSurfPoint.z;
    }

    // fall back to other point data to find a decent scene offset value
    if (MathHelper::IsFuzzyEqual(m_sceneOriginOffsetX, 0.0) &&
        MathHelper::IsFuzzyEqual(m_sceneOriginOffsetY, 0.0))
    {
        if (landXMLModel.m_landxmlPoints.size() > 0)
        {
            auto cgIter = landXMLModel.m_landxmlPoints.begin();

            m_sceneOriginOffsetX = cgIter->second.x;
            m_sceneOriginOffsetY = cgIter->second.y;
            m_sceneOriginOffsetZ = cgIter->second.z;
        }
        else if (landXMLModel.m_landxmlPlanFeatures.size() > 0 && landXMLModel.m_landxmlPlanFeatures[0].m_polylinePoints.size() > 0)
        {
            m_sceneOriginOffsetX = landXMLModel.m_landxmlPlanFeatures[0].m_polylinePoints[0].x;
            m_sceneOriginOffsetY = landXMLModel.m_landxmlPlanFeatures[0].m_polylinePoints[0].y;
            m_sceneOriginOffsetZ = landXMLModel.m_landxmlPlanFeatures[0].m_polylinePoints[0].z;
        }
        else if (landXMLModel.m_landxmlAlignments.size() > 0 && landXMLModel.m_landxmlAlignments[0].m_polylinePoints.size() > 0)
        {
            m_sceneOriginOffsetX = landXMLModel.m_landxmlAlignments[0].m_polylinePoints[0].x;
            m_sceneOriginOffsetY = landXMLModel.m_landxmlAlignments[0].m_polylinePoints[0].y;
            m_sceneOriginOffsetZ = landXMLModel.m_landxmlAlignments[0].m_polylinePoints[0].z;
        }
        else if (landXMLModel.m_landxmlParcels.size() > 0 && landXMLModel.m_landxmlParcels[0].m_polylinePoints.size() > 0)
        {
            m_sceneOriginOffsetX = landXMLModel.m_landxmlParcels[0].m_polylinePoints[0].x;
            m_sceneOriginOffsetY = landXMLModel.m_landxmlParcels[0].m_polylinePoints[0].y;
            m_sceneOriginOffsetZ = landXMLModel.m_landxmlParcels[0].m_polylinePoints[0].z;
        }
    }
}

void LandXMLModel2glTF::BuildGLTFMaterialTable(const LandXMLModel& landXMLModel, GLTFModel& gltfModel)
{
    // build the GLTF material table
    for (auto LXMat = landXMLModel.m_landXMLMaterials.m_MaterialMap.begin(); LXMat != landXMLModel.m_landXMLMaterials.m_MaterialMap.end(); LXMat++)
    {
        GLTFSurfaceMaterial matToAdd;// = new GLTFSurfaceMaterial();
        unsigned int idToUse = 0;

        if (LXMat->second.m_ID > 0)
        {
            idToUse = LXMat->second.m_ID - 1;
        }

        char idAsChar[50] = { 0 };
        snprintf(idAsChar, 50, "%d", idToUse);

        matToAdd.m_material.id = idAsChar;
        matToAdd.m_material.name = LXMat->second.m_name;

        LXParser::LXColor2RGB(LXMat->second.m_RGBColorStr, matToAdd.m_color.r, matToAdd.m_color.g, matToAdd.m_color.b);
        matToAdd.m_color.a = 1.0f;

        matToAdd.m_material.metallicRoughness.baseColorFactor = matToAdd.m_color;

        gltfModel.gltfMeshMaterials.push_back(matToAdd);
    }
}

bool LandXMLModel2glTF::BuildGLTFSurfaceModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel)
{
    bool retVal = false;

    // build the GLTF Surface Meshes
    for (LandXMLSurface* LXSurf : landXMLModel.m_landxmlSurfaces)
    {
        GLTFSurfaceModel* gltfSurfModel = new GLTFSurfaceModel();

        if (!gltfSurfModel)
        {
            return false;
        }

        gltfSurfModel->name = LXSurf->m_name;

        // build the surface meshes
        for (unsigned int pid = 1; pid <= LXSurf->m_surfacePointCount; pid++)
        {
            LandXMLPoint3D lxPnt = LXSurf->m_surfacePoints[pid];
            std::vector<float> glpnt1(3U);

            //if (m_wgsTrans)
            //{
            //    m_wgsTrans->Transform(1, &lxPnt.x, &lxPnt.y, &lxPnt.z, &success);
            //    lxPnt.z = lxPnt.z * m_unitConversionToWG84;
            //}

            lxPnt.x -= m_sceneOriginOffsetX;
            lxPnt.y -= m_sceneOriginOffsetY;
            lxPnt.z -= m_sceneOriginOffsetZ;

            lxPnt.x = lxPnt.x * m_unitConversionToWG84;
            lxPnt.y = lxPnt.y * m_unitConversionToWG84;
            lxPnt.z = lxPnt.z * m_unitConversionToWG84;

            MathHelper::CopyLXTOGLTFPoint(lxPnt, glpnt1);

            if (glpnt1.size() > 2)
            {
                gltfSurfModel->gltfMeshPoints.push_back(glpnt1[0]);
                gltfSurfModel->gltfMeshPoints.push_back(glpnt1[1]);
                gltfSurfModel->gltfMeshPoints.push_back(glpnt1[2]);
            }
        }

        int txCount = 0;
        for (auto txb = LXSurf->m_textureBoundaries.begin(); txb != LXSurf->m_textureBoundaries.end(); txb++)
        {
            std::vector<unsigned int> localgltfMeshIndices;

            for (LandXMLSurfaceFace lxFace : txb->m_surfaceFaces)
            {
                // LandXML uses 1 based index values, glTF uses zero based indices.
                unsigned int a = lxFace.m_pointIndices[0] - 1;
                unsigned int b = lxFace.m_pointIndices[1] - 1;
                unsigned int c = lxFace.m_pointIndices[2] - 1;

                localgltfMeshIndices.push_back(a);
                localgltfMeshIndices.push_back(b);
                localgltfMeshIndices.push_back(c);
            }

            gltfSurfModel->gltfSubMeshIndexIDs[txCount] = localgltfMeshIndices;
            gltfSurfModel->gltfSubMeshIndicesMaterialMap[txCount] = (txb->m_materialID - 1);

            txCount++;
        }

        gltfModel.gltfSurfaceModels.push_back(gltfSurfModel);
        retVal = true;
    }

    return retVal;
}

bool LandXMLModel2glTF::BuildGLTFPolylineModels(const LandXMLModel& landXMLModel, GLTFModel& gltfModel)
{
    bool retVal = false;

    double planarElev = 0.0;

    for (LandXMLSurface* lxsurf : landXMLModel.m_landxmlSurfaces)
    {
        if (lxsurf->m_maxSurfPoint.z > planarElev)
        {
            planarElev = lxsurf->m_maxSurfPoint.z;
        }
    }

    planarElev += 50.0;

    // build the GLTF Polyline Meshes
    for (LandXMLPolyline LXPlan : landXMLModel.m_landxmlPlanFeatures)
    {
        GLTFPolylineModel* newPoly = BuildGLTFPolyline(LXPlan, planarElev);

        if (newPoly)
        {
            gltfModel.gltfMultiPolyModel.gltfPolylines.push_back(newPoly);
        }
    }

    for (LandXMLPolyline LXAlign : landXMLModel.m_landxmlAlignments)
    {
        GLTFPolylineModel* newPoly = BuildGLTFPolyline(LXAlign, planarElev);

        if (newPoly)
        {
            gltfModel.gltfMultiPolyModel.gltfPolylines.push_back(newPoly);
        }
    }

    for (LandXMLPolyline LXParcel : landXMLModel.m_landxmlParcels)
    {
        GLTFPolylineModel* newPoly = BuildGLTFPolyline(LXParcel, planarElev);

        if (newPoly)
        {
            gltfModel.gltfMultiPolyModel.gltfPolylines.push_back(newPoly);
        }
    }

    unsigned int indexOffset = 0;

    for (GLTFPolylineModel* poly : gltfModel.gltfMultiPolyModel.gltfPolylines)
    {
        indexOffset = (unsigned int)gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.size() / 3U;

        gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.reserve(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.size() + poly->gltfPolylinePoints.size());
        gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.insert(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.end(), poly->gltfPolylinePoints.begin(), poly->gltfPolylinePoints.end());

        for (unsigned int v = 0; v < poly->gltfPolylineIndexIDs.size(); ++v)
        {
            poly->gltfPolylineIndexIDs[v] += indexOffset;
        }
    }

    return retVal;
}

GLTFPolylineModel* LandXMLModel2glTF::BuildGLTFPolyline(LandXMLPolyline& LXPoly, double& planeElev)
{
    GLTFPolylineModel* gltfPolyModel = new GLTFPolylineModel();

    if (!gltfPolyModel)
    {
        return nullptr;
    }

    gltfPolyModel->name = LXPoly.m_name;

    bool isDraped = false;
    double aveElev = 0.0;

    if (LXPoly.m_polylinePoints.size() > 2)
    {
        for (unsigned int pid = 0; pid < 3; pid++)
        {
            aveElev += LXPoly.m_polylinePoints[pid].z;
        }

        aveElev /= 3;
        if (!MathHelper::IsFuzzyEqual(aveElev, 0.0))
        {
            isDraped = true;
        }
    }

    // build the polyline mesh
    for (unsigned int pid = 0; pid < LXPoly.m_polylinePoints.size(); pid++)
    {
        LandXMLPoint3D lxPnt = LXPoly.m_polylinePoints[pid];

        // set polyline planar elevation about highest surface, if not draped
        if (!isDraped)
        {
            lxPnt.z = planeElev;
        }

        std::vector<float> glpnt1(3U);

        // TODO: Need to figure out GLTF world coordinates
        //if (m_wgsTrans)
        //{
        //    m_wgsTrans->Transform(1, &lxPnt.x, &lxPnt.y, &lxPnt.z, &success);
        //    lxPnt.z = lxPnt.z * m_unitConversionToWG84;
        //}

        lxPnt.x -= m_sceneOriginOffsetX;
        lxPnt.y -= m_sceneOriginOffsetY;
        lxPnt.z -= m_sceneOriginOffsetZ;

        lxPnt.x = lxPnt.x * m_unitConversionToWG84;
        lxPnt.y = lxPnt.y * m_unitConversionToWG84;
        lxPnt.z = lxPnt.z * m_unitConversionToWG84;

        MathHelper::CopyLXTOGLTFPoint(lxPnt, glpnt1);

        if (glpnt1.size() > 2)
        {
            gltfPolyModel->gltfPolylinePoints.push_back(glpnt1[0]);
            gltfPolyModel->gltfPolylinePoints.push_back(glpnt1[1]);
            gltfPolyModel->gltfPolylinePoints.push_back(glpnt1[2]);
        }
    }

    size_t pointCount = gltfPolyModel->gltfPolylinePoints.size() / 3U;

    for (unsigned int i = 0U; i < (pointCount - 1); ++i)
    {
        gltfPolyModel->gltfPolylineIndexIDs.push_back(i);
        gltfPolyModel->gltfPolylineIndexIDs.push_back(i + 1);
    }

    gltfPolyModel->m_materialId = LXPoly.m_materialID - 1;

    return gltfPolyModel;
}

void LandXMLModel2glTF::AddGLTFSurfaceMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::BufferBuilder& bufferBuilder)
{
    for (GLTFSurfaceModel* gltfSurfModel : gltfModel.gltfSurfaceModels)
    {
        // Create all the resource data (e.g. triangle indices and
        // vertex positions) that will be written to the binary buffer
        const char* bufferId = nullptr;

        // Specify the 'special' GLB buffer ID. This informs the GLBResourceWriter that it should use
        // the GLB container's binary chunk (usually the desired buffer location when creating GLBs)
        if (dynamic_cast<const Microsoft::glTF::GLBResourceWriter*>(&bufferBuilder.GetResourceWriter()))
        {
            bufferId = Microsoft::glTF::GLB_BUFFER_ID;
        }

        // Create a Buffer - it will be the 'current' Buffer that all the BufferViews
        // created by this BufferBuilder will automatically reference
        bufferBuilder.AddBuffer(bufferId);

        // ******* Surface Points ***************************************************************
        // Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
        bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);

        std::vector<float> minValues(3U, FLT_MAX);
        std::vector<float> maxValues(3U, -FLT_MAX);

        const size_t positionCount = gltfSurfModel->gltfMeshPoints.size();

        // Accessor min/max properties must be set for vertex position data so calculate them here
        for (size_t i = 0U, j = 0U; (i + j) < positionCount; i += 3, j = (i % 3U))
        {
            minValues[0] = std::min<float>(gltfSurfModel->gltfMeshPoints[i], minValues[0]);
            minValues[1] = std::min<float>(gltfSurfModel->gltfMeshPoints[i + 1], minValues[1]);
            minValues[2] = std::min<float>(gltfSurfModel->gltfMeshPoints[i + 2], minValues[2]);

            maxValues[0] = std::max<float>(gltfSurfModel->gltfMeshPoints[i], maxValues[0]);
            maxValues[1] = std::max<float>(gltfSurfModel->gltfMeshPoints[i + 1], maxValues[1]);
            maxValues[2] = std::max<float>(gltfSurfModel->gltfMeshPoints[i + 2], maxValues[2]);
        }

        gltfSurfModel->accessorIdPositions.push_back(bufferBuilder.AddAccessor(gltfSurfModel->gltfMeshPoints, { Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id);
        // ***************************************************************************************

        // ******* Sub Surface Mesh Indices *******************************************************
        for (unsigned int acc = 0; acc < gltfSurfModel->gltfSubMeshIndexIDs.size(); acc++)
        {
            // Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
            // data) - it will be the 'current' BufferView that all the Accessors created by this
            // BufferBuilder will automatically reference

            bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ELEMENT_ARRAY_BUFFER);

            // Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
            gltfSurfModel->accessorIdIndices.push_back(bufferBuilder.AddAccessor(gltfSurfModel->gltfSubMeshIndexIDs[acc], { Microsoft::glTF::TYPE_SCALAR, Microsoft::glTF::COMPONENT_UNSIGNED_INT }).id);
        }
        // ***************************************************************************************

    }
}

void LandXMLModel2glTF::AddGLTFSurfaceMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene)
{
    for (GLTFSurfaceModel* gltfSurfModel : gltfModel.gltfSurfaceModels)
    {
        std::vector<Microsoft::glTF::MeshPrimitive> surfaceSubMeshes;

        for (unsigned int acc = 0; acc < gltfSurfModel->gltfSubMeshIndexIDs.size(); acc++)
        {
            Microsoft::glTF::MeshPrimitive meshPrimitive;

            meshPrimitive.mode = Microsoft::glTF::MESH_TRIANGLES;
            meshPrimitive.materialId = gltfModel.gltfMeshMaterials[gltfSurfModel->gltfSubMeshIndicesMaterialMap[acc]].m_material.id;
            meshPrimitive.indicesAccessorId = gltfSurfModel->accessorIdIndices[acc];
            meshPrimitive.attributes[Microsoft::glTF::ACCESSOR_POSITION] = gltfSurfModel->accessorIdPositions[0];

            surfaceSubMeshes.push_back(meshPrimitive);
        }

        // Construct a Mesh and add the MeshPrimitive as a child
        Microsoft::glTF::Mesh mesh;
        mesh.name = gltfSurfModel->name;

        for (Microsoft::glTF::MeshPrimitive& surfaceSubMesh : surfaceSubMeshes)
        {
            mesh.primitives.push_back(std::move(surfaceSubMesh));
        }

        // Add it to the Document and store the generated ID
        auto meshId = document.meshes.Append(std::move(mesh), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

        // Construct a Node adding a reference to the Mesh
        Microsoft::glTF::Node node;
        node.meshId = meshId;
        node.name = "LandXML Surface :" + gltfSurfModel->name;

        // Add it to the Document and store the generated ID
        auto nodeId = document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

        scene.nodes.push_back(nodeId);
    }
}

void LandXMLModel2glTF::AddGLTFPolylineMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::BufferBuilder& bufferBuilder)
{
    if (gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.size() == 0 ||
        gltfModel.gltfMultiPolyModel.gltfPolylines.size() == 0)
    {
        return;
    }

    // Create all the resource data (e.g. triangle indices and
    // vertex positions) that will be written to the binary buffer
    const char* bufferId = nullptr;

    // Specify the 'special' GLB buffer ID. This informs the GLBResourceWriter that it should use
    // the GLB container's binary chunk (usually the desired buffer location when creating GLBs)
    if (dynamic_cast<const Microsoft::glTF::GLBResourceWriter*>(&bufferBuilder.GetResourceWriter()))
    {
        bufferId = Microsoft::glTF::GLB_BUFFER_ID;
    }

    // Create a Buffer - it will be the 'current' Buffer that all the BufferViews
    // created by this BufferBuilder will automatically reference
    bufferBuilder.AddBuffer(bufferId);

    // ******* Polyline Points ***************************************************************
    // Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);

    std::vector<float> maxValues(3U, -FLT_MAX);
    std::vector<float> minValues(3U, FLT_MAX);

    const size_t positionCount = gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints.size();

    // Accessor min/max properties must be set for vertex position data so calculate them here
    for (size_t i = 0U, j = 0U; (i + j) < positionCount; i += 3, j = (i % 3U))
    {
        minValues[0] = std::min<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i], minValues[0]);
        minValues[1] = std::min<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i + 1], minValues[1]);
        minValues[2] = std::min<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i + 2], minValues[2]);

        maxValues[0] = std::max<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i], maxValues[0]);
        maxValues[1] = std::max<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i + 1], maxValues[1]);
        maxValues[2] = std::max<float>(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints[i + 2], maxValues[2]);
    }

    gltfModel.gltfMultiPolyModel.accessorId = (bufferBuilder.AddAccessor(gltfModel.gltfMultiPolyModel.gltfMultiPolylinePoints, { Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id);
    // ***************************************************************************************

    for (GLTFPolylineModel* gltfPolylineModel : gltfModel.gltfMultiPolyModel.gltfPolylines)
    {

        // ******* Polyline Mesh Indices *******************************************************
        // Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
        // data) - it will be the 'current' BufferView that all the Accessors created by this
        // BufferBuilder will automatically reference

        bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ELEMENT_ARRAY_BUFFER);

        // Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
        gltfPolylineModel->accessorIdPolylineIndex = (bufferBuilder.AddAccessor(gltfPolylineModel->gltfPolylineIndexIDs, { Microsoft::glTF::TYPE_SCALAR, Microsoft::glTF::COMPONENT_UNSIGNED_INT }).id);
        // ***************************************************************************************

    }
}

void LandXMLModel2glTF::AddGLTFPolylineMeshes(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::Scene& scene)
{
    for (GLTFPolylineModel* gltfPolylineModel : gltfModel.gltfMultiPolyModel.gltfPolylines)
    {
        std::vector<Microsoft::glTF::MeshPrimitive> polylineMeshes;

        Microsoft::glTF::MeshPrimitive meshPrimitive;
        meshPrimitive.mode = Microsoft::glTF::MESH_LINES;
        meshPrimitive.materialId = "0";

        if (gltfPolylineModel->m_materialId < gltfModel.gltfMeshMaterials.size())
        {
            meshPrimitive.materialId = gltfModel.gltfMeshMaterials[gltfPolylineModel->m_materialId].m_material.id;
        }

        meshPrimitive.indicesAccessorId = gltfPolylineModel->accessorIdPolylineIndex;
        meshPrimitive.attributes[Microsoft::glTF::ACCESSOR_POSITION] = gltfModel.gltfMultiPolyModel.accessorId;

        polylineMeshes.push_back(meshPrimitive);

        // Construct a Mesh and add the MeshPrimitive as a child
        Microsoft::glTF::Mesh mesh;
        mesh.name = gltfPolylineModel->name;

        for (Microsoft::glTF::MeshPrimitive& polylineMesh : polylineMeshes)
        {
            mesh.primitives.push_back(std::move(polylineMesh));
        }

        // Add it to the Document and store the generated ID
        auto meshId = document.meshes.Append(std::move(mesh), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

        // Construct a Node adding a reference to the Mesh
        Microsoft::glTF::Node node;
        node.meshId = meshId;
        node.name = "LandXML Polyline :" + gltfPolylineModel->name;

        // Add it to the Document and store the generated ID
        auto nodeId = document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

        scene.nodes.push_back(nodeId);
    }
}

void LandXMLModel2glTF::WriteGLTFFile(Microsoft::glTF::Document& document, GLTFModel& gltfModel, std::filesystem::path& glTFFilename)
{
    // Pass the absolute path, without the filename, to the stream writer
    auto streamWriter = std::make_unique<StreamWriter>(glTFFilename.parent_path());

    std::filesystem::path pathFile = glTFFilename.filename();
    std::filesystem::path pathFileExt = pathFile.extension();

    auto MakePathExt = [](const std::string& ext)
        {
            return "." + ext;
        };

    std::unique_ptr<Microsoft::glTF::ResourceWriter> resourceWriter;

    // If the file has a '.gltf' extension then create a GLTFResourceWriter
    if (pathFileExt == MakePathExt(Microsoft::glTF::GLTF_EXTENSION))
    {
        resourceWriter = std::make_unique<Microsoft::glTF::GLTFResourceWriter>(std::move(streamWriter));

        Microsoft::glTF::GLTFResourceWriter* localGLTFWriter = dynamic_cast<Microsoft::glTF::GLTFResourceWriter*>(resourceWriter.get());

        if (localGLTFWriter)
        {
            std::string bufferURI = glTFFilename.filename().replace_extension("").string() + "_Surf_";
            localGLTFWriter->SetUriPrefix(bufferURI);
        }

    }

    // If the file has a '.glb' extension then create a GLBResourceWriter. This class derives
    // from GLTFResourceWriter and adds support for writing manifests to a GLB container's
    // JSON chunk and resource data to the binary chunk.
    if (pathFileExt == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
    {
        resourceWriter = std::make_unique<Microsoft::glTF::GLBResourceWriter>(std::move(streamWriter));
    }

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    Microsoft::glTF::Scene gltfScene;
    Microsoft::glTF::BufferBuilder bufferBuilder(std::move(resourceWriter));
    std::string manifest;

    AddGLTFSurfaceMeshBuffers(gltfModel, bufferBuilder);
    AddGLTFSurfaceMeshes(gltfModel, document, gltfScene);

    Microsoft::glTF::ResourceWriter& localGLTFResourceWriter = bufferBuilder.GetResourceWriter();
    Microsoft::glTF::GLTFResourceWriter* localGLTFWriter = dynamic_cast<Microsoft::glTF::GLTFResourceWriter*>(&localGLTFResourceWriter);
    if (localGLTFWriter)
    {
        std::string bufferURI = glTFFilename.filename().replace_extension("").string() + "_Poly_";
        localGLTFWriter->SetUriPrefix(bufferURI);
    }

    AddGLTFPolylineMeshBuffers(gltfModel, bufferBuilder);
    AddGLTFPolylineMeshes(gltfModel, document, gltfScene);

    // Add Materials from LandXML material table
    for (auto gltfMat = gltfModel.gltfMeshMaterials.begin(); gltfMat != gltfModel.gltfMeshMaterials.end(); gltfMat++)
    {
        // Add to the Document and store the generated ID
        document.materials.Append(std::move(gltfMat->m_material), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);
    }

    document.SetDefaultScene(std::move(gltfScene), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);

    // Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
    // the Document. Note that after this point, no further calls should be made to BufferBuilder
    bufferBuilder.Output(document);

    try
    {
        // Serialize the glTF Document into a JSON manifest
        manifest = Microsoft::glTF::Serialize(document, Microsoft::glTF::SerializeFlags::Pretty);
    }
    catch (const Microsoft::glTF::GLTFException& ex)
    {
        std::stringstream ss;

        ss << "Microsoft::glTF::Serialize failed: ";
        ss << ex.what();

        throw std::runtime_error(ss.str());
    }

    auto& gltfResourceWriter = bufferBuilder.GetResourceWriter();

    if (auto glbResourceWriter = dynamic_cast<Microsoft::glTF::GLBResourceWriter*>(&gltfResourceWriter))
    {
        glbResourceWriter->Flush(manifest, pathFile.u8string()); // A GLB container isn't created until the GLBResourceWriter::Flush member function is called
    }
    else
    {
        gltfResourceWriter.WriteExternal(pathFile.u8string(), manifest); // Binary resources have already been written, just need to write the manifest
    }
}
}

