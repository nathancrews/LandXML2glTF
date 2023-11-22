
#include "LandXMLModel2glTF.h"
#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"
#include "Writers/GLTFWriter.h"
#include <sstream>

namespace LANDXML2GLTF
{

bool LandXMLModel2glTF::Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename)
{
    bool retval = false;
    LandXMLModel landXMLModel;
    OGRSpatialReference* landXMLSpatialRef = nullptr;
    OGRCoordinateTransformation* wgsTrans = nullptr;
    tinyxml2::XMLDocument* LXDocument = nullptr;

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

    LXParser LXHelper;

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

    landXMLSpatialRef = new OGRSpatialReference();

    OGRErr tStat = landXMLSpatialRef->importFromWkt(landXMLModel.m_wktString.c_str());

    if (tStat == OGRERR_NONE)
    {
        wgsTrans = MathHelper::GetWGS84CoordTransform(*landXMLSpatialRef);
        char* units = nullptr;
        m_unitConversionToWG84 = landXMLSpatialRef->GetLinearUnits(&units);
    }

    std::cout << "Parsing and building LandXML model...\n";

    if (!LXHelper.ParseLandXMLFile(LXDocument, landXMLModel))
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
        retval = CreateGLTFModel(landXMLModel, glTFDoc, gltfModel);

        std::cout << "Writing glTF file: " << glTFFilename << "\n";
        WriteGLTFFile(glTFDoc, gltfModel, std::filesystem::path(glTFFilename));
    }
    catch (...)
    {

    }

    // cleanup memory
    delete LXDocument;

    for (GLTFSurfaceModel* gltfSurfModel : gltfModel.gltfSurfaceModels)
    {
        delete gltfSurfModel;
    }

    return retval;
}

bool LandXMLModel2glTF::CreateGLTFModel(const LandXMLModel& landXMLModel, Microsoft::glTF::Document& glTFDocument, GLTFModel& gltfModel)
{
    bool retVal = false;
    int success = 0;
    double sceneOriginOffsetX = 0.0;
    double sceneOriginOffsetY = 0.0;
    double sceneOriginOffsetZ = 0.0;

    // Set the scene origin offset value
    if (landXMLModel.m_landxmlSurfaces.front())
    {
        sceneOriginOffsetX = landXMLModel.m_landxmlSurfaces.front()->m_minSurfPoint.x;
        sceneOriginOffsetY = landXMLModel.m_landxmlSurfaces.front()->m_minSurfPoint.y;
        sceneOriginOffsetZ = landXMLModel.m_landxmlSurfaces.front()->m_minSurfPoint.z;
    }

    // build the GLTF material table
    for (auto LXMat = landXMLModel.m_landXMLMaterials.m_MaterialMap.begin(); LXMat != landXMLModel.m_landXMLMaterials.m_MaterialMap.end(); LXMat++)
    {
        GLTFSurfaceMaterial matToAdd;// = new GLTFSurfaceMaterial();
        unsigned int idToUse = 0;

        if (LXMat->second.m_ID > 0)
        {
            idToUse = LXMat->second.m_ID - 1;
        }

        char idAsChar[4] = { 0,0,0,0 };
        sprintf(idAsChar, "%d", idToUse);

        matToAdd.m_material.id = idAsChar;
        matToAdd.m_material.name = LXMat->second.m_name;

        LXParser::LXColor2RGB(LXMat->second.m_RGBColorStr, matToAdd.m_color.r, matToAdd.m_color.g, matToAdd.m_color.b);
        matToAdd.m_color.a = 1.0f;

        matToAdd.m_material.metallicRoughness.baseColorFactor = matToAdd.m_color;

        gltfModel.gltfSubMeshMaterials.push_back(matToAdd);
    }

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

            lxPnt.x -= sceneOriginOffsetX;
            lxPnt.y -= sceneOriginOffsetY;
            lxPnt.z -= sceneOriginOffsetZ;

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

        int m = 0;
        std::vector<UINT> minValues(3U, UINT_MAX);
        std::vector<UINT> maxValues(3U, 0U);

        const size_t faceCount = LXSurf->m_textureBoundaries.front().m_surfaceFaces.size();

        // Accessor min/max properties must be set for vertex position data so calculate them here
        for (size_t i = 0U, j = 0U; i < faceCount; ++i, j = (i % 3U))
        {
            minValues[j] = std::min<UINT>(LXSurf->m_textureBoundaries.front().m_surfaceFaces[i].m_pointIndices[j], minValues[j]);
            maxValues[j] = std::max<UINT>(LXSurf->m_textureBoundaries.front().m_surfaceFaces[i].m_pointIndices[j], maxValues[j]);
        }

        int txCount = 0;
        for (auto txb = LXSurf->m_textureBoundaries.begin(); txb != LXSurf->m_textureBoundaries.end(); txb++)
        {
            std::vector<UINT> localgltfMeshIndices;

            for (LandXMLSurfaceFace lxFace : txb->m_surfaceFaces)
            {
                // LandXML uses 1 based index values, glTF uses zero based indices.
                UINT a = lxFace.m_pointIndices[0] - 1;
                UINT b = lxFace.m_pointIndices[1] - 1;
                UINT c = lxFace.m_pointIndices[2] - 1;

                localgltfMeshIndices.push_back(a);
                localgltfMeshIndices.push_back(b);
                localgltfMeshIndices.push_back(c);
            }

            gltfSurfModel->gltfSubMeshIndexIDs[txCount] = localgltfMeshIndices;
            gltfSurfModel->gltfSubMeshIndicesMaterialMap[txCount] = (txb->m_materialID - 1);

            txCount++;
        }

        gltfModel.gltfSurfaceModels.push_back(gltfSurfModel);
    }

    return retVal;
}

void LandXMLModel2glTF::AddGLTFMeshBuffers(GLTFModel& gltfModel, Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder)
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
        std::vector<float> maxValues(3U, FLT_MIN);

        const size_t positionCount = gltfSurfModel->gltfMeshPoints.size();

        // Accessor min/max properties must be set for vertex position data so calculate them here
        for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U))
        {
            minValues[j] = std::min<float>(gltfSurfModel->gltfMeshPoints[i], minValues[j]);
            maxValues[j] = std::max<float>(gltfSurfModel->gltfMeshPoints[i], maxValues[j]);
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

    // Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
    // the Document. Note that after this point, no further calls should be made to BufferBuilder
    bufferBuilder.Output(document);
}

void LandXMLModel2glTF::AddGLTFMesh(GLTFModel& gltfModel, Microsoft::glTF::Document& document)
{
    Microsoft::glTF::Scene scene;
    static bool doneOnce = false;

    for (GLTFSurfaceModel* gltfSurfModel : gltfModel.gltfSurfaceModels)
    {
        std::vector<Microsoft::glTF::MeshPrimitive> surfaceSubMeshes;

        for (unsigned int acc = 0; acc < gltfSurfModel->gltfSubMeshIndexIDs.size(); acc++)
        {
            Microsoft::glTF::MeshPrimitive meshPrimitive;

            meshPrimitive.mode = Microsoft::glTF::MESH_TRIANGLES;
            meshPrimitive.materialId = gltfModel.gltfSubMeshMaterials[gltfSurfModel->gltfSubMeshIndicesMaterialMap[acc]].m_material.id;
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

    // Add Materials from LandXML material table ONCE
    for (auto gltfMat = gltfModel.gltfSubMeshMaterials.begin(); gltfMat != gltfModel.gltfSubMeshMaterials.end(); gltfMat++)
    {
        // Add to the Document and store the generated ID
        document.materials.Append(std::move(gltfMat->m_material), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);
    }

    // Add it to the Document, using a utility method that also sets the Scene as the Document's default
    document.SetDefaultScene(std::move(scene), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);
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
            std::string bufferURI = glTFFilename.filename().replace_extension("").string() + "_";
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

    Microsoft::glTF::BufferBuilder bufferBuilder(std::move(resourceWriter));

    AddGLTFMeshBuffers(gltfModel, document, bufferBuilder);
    AddGLTFMesh(gltfModel, document);

    std::string manifest;

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

