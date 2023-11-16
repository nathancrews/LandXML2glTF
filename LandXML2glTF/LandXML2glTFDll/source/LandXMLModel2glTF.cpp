
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

    if (false == std::filesystem::exists(InLandXMLFilename))
    {
        return retval;
    }

    m_LXDocument = new tinyxml2::XMLDocument(true, COLLAPSE_WHITESPACE);

    if (!m_LXDocument)
    {
        return retval;
    }

    m_LXDocument->LoadFile(InLandXMLFilename.c_str());

    LXParser LXHelper;

    LXHelper.ParseLandXMLHeader(m_LXDocument, m_landXMLModel);

    if (m_landXMLModel.m_units.m_linearUnitString.compare("USSurveyFoot"))
    {
        m_unitConversionToWG84 = USFT2M;
    }
    else if (m_landXMLModel.m_units.m_linearUnitString.compare("feet"))
    {
        m_unitConversionToWG84 = IFT2M;
    }

    m_landXMLSpatialRef = new OGRSpatialReference();

    OGRErr tStat = m_landXMLSpatialRef->importFromWkt(m_landXMLModel.m_wktString.c_str());

    if (tStat == OGRERR_NONE)
    {
        m_wgsTrans = MathHelper::GetWGS84CoordTransform(*m_landXMLSpatialRef);
        char* units = nullptr;
        m_unitConversionToWG84 = m_landXMLSpatialRef->GetLinearUnits(&units);

    }

    LXHelper.ParseLandXMLFile(m_LXDocument, m_landXMLModel);


    Microsoft::glTF::Document glTFDoc;
 
    CreateGLTFModel(m_landXMLModel, glTFDoc);

    WriteGLTFFile(glTFDoc, std::filesystem::path(glTFFilename));


    // cleanup memory
    delete m_LXDocument;

    return retval;
}

bool LandXMLModel2glTF::CreateGLTFModel(const LandXMLModel& landXMLModel, Microsoft::glTF::Document& glTFDocument)
{
    bool retVal = false;
    int success = 0;

    for (LandXMLSurface* LXSurf : landXMLModel.m_landxmlSurfaces)
    {
        for (LandXMLPoint3D lxPnt : LXSurf->m_surfacePoints)
        {
            std::vector<float> glpnt1(3U);

            //if (m_wgsTrans)
            //{
            //    m_wgsTrans->Transform(1, &lxPnt.x, &lxPnt.y, &lxPnt.z, &success);
            //    lxPnt.z = lxPnt.z * m_unitConversionToWG84;
            //}

            lxPnt.x -= LXSurf->m_minSurfPoint.x;
            lxPnt.y -= LXSurf->m_minSurfPoint.y;
            lxPnt.z -= LXSurf->m_minSurfPoint.z;

            lxPnt.x = lxPnt.x * m_unitConversionToWG84;
            lxPnt.y = lxPnt.y * m_unitConversionToWG84;
            lxPnt.z = lxPnt.z * m_unitConversionToWG84;

            MathHelper::CopyLXTOGLTFPoint(lxPnt, glpnt1);

            if (glpnt1.size() > 2)
            {
                gltfMeshPoints.push_back(glpnt1[0]);
                gltfMeshPoints.push_back(glpnt1[1]);
                gltfMeshPoints.push_back(glpnt1[2]);
            }

        }


        int m = 0;
        std::vector<UINT> minValues(3U, UINT_MAX);
        std::vector<UINT> maxValues(3U, 0U);

        const size_t faceCount = LXSurf->m_surfaceMeshes[m]->m_surfaceFaces.size();

        // Accessor min/max properties must be set for vertex position data so calculate them here
        for (size_t i = 0U, j = 0U; i < faceCount; ++i, j = (i % 3U))
        {
            minValues[j] = std::min<UINT>(LXSurf->m_surfaceMeshes[m]->m_surfaceFaces[i].m_pointIndices[j], minValues[j]);
            maxValues[j] = std::max<UINT>(LXSurf->m_surfaceMeshes[m]->m_surfaceFaces[i].m_pointIndices[j], maxValues[j]);
        }

//        for (int m = 1; m < LXSurf->m_surfaceMeshes.size(); m++)
        {
            
            if (LXSurf->m_surfaceMeshes[m])
            {
                for (LandXMLSurfaceFace lxFace : LXSurf->m_surfaceMeshes[m]->m_surfaceFaces)
                {
                    UINT a = lxFace.m_pointIndices[0];
                    UINT b = lxFace.m_pointIndices[1];
                    UINT c = lxFace.m_pointIndices[2];

                    gltfMeshIndices.push_back(a - 1);
                    gltfMeshIndices.push_back(b - 1);
                    gltfMeshIndices.push_back(c - 1);
                }

                gltfSubMeshIndices.push_back(gltfMeshIndices);
            }
        }

    }

    return retVal;
}

void LandXMLModel2glTF::AddGLTFMeshBuffers(Microsoft::glTF::Document& document, Microsoft::glTF::BufferBuilder& bufferBuilder, std::string& accessorIdIndices, std::string& accessorIdPositions)
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

    // Create a BufferView with a target of ELEMENT_ARRAY_BUFFER (as it will reference index
    // data) - it will be the 'current' BufferView that all the Accessors created by this
    // BufferBuilder will automatically reference
    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ELEMENT_ARRAY_BUFFER);

    // Copy the Accessor's id - subsequent calls to AddAccessor may invalidate the returned reference
    accessorIdIndices = bufferBuilder.AddAccessor(gltfMeshIndices, { Microsoft::glTF::TYPE_SCALAR, Microsoft::glTF::COMPONENT_UNSIGNED_INT }).id;

    // Create a BufferView with target ARRAY_BUFFER (as it will reference vertex attribute data)
    bufferBuilder.AddBufferView(Microsoft::glTF::BufferViewTarget::ARRAY_BUFFER);

    std::vector<float> minValues(3U, FLT_MAX);
    std::vector<float> maxValues(3U, FLT_MIN);

    const size_t positionCount = gltfMeshPoints.size();

    // Accessor min/max properties must be set for vertex position data so calculate them here
    for (size_t i = 0U, j = 0U; i < positionCount; ++i, j = (i % 3U))
    {
        minValues[j] = std::min<float>(gltfMeshPoints[i], minValues[j]);
        maxValues[j] = std::max<float>(gltfMeshPoints[i], maxValues[j]);
    }

    accessorIdPositions = bufferBuilder.AddAccessor(gltfMeshPoints, { Microsoft::glTF::TYPE_VEC3, Microsoft::glTF::COMPONENT_FLOAT, false, std::move(minValues), std::move(maxValues) }).id;

    // Add all of the Buffers, BufferViews and Accessors that were created using BufferBuilder to
    // the Document. Note that after this point, no further calls should be made to BufferBuilder
    bufferBuilder.Output(document);
}

void LandXMLModel2glTF::AddGLTFMesh(Microsoft::glTF::Document& document, const std::string& accessorIdIndices, const std::string& accessorIdPositions)
{
    // Create a very simple glTF Document with the following hierarchy:
    //  Scene
    //     Node
    //       Mesh (Triangle)
    //         MeshPrimitive
    //           Material (Blue)
    // 
    // A Document can be constructed top-down or bottom up. However, if constructed top-down
    // then the IDs of child entities must be known in advance, which prevents using the glTF
    // SDK's automatic ID generation functionality.

    // Construct a Material
    Microsoft::glTF::Material material;
    material.metallicRoughness.baseColorFactor = Microsoft::glTF::Color4(0.0f, 1.0f, 0.0f, 1.0f);
    material.metallicRoughness.metallicFactor = 0.2f;
    material.metallicRoughness.roughnessFactor = 0.4f;
    material.doubleSided = true;

    // Add it to the Document and store the generated ID
    auto materialId = document.materials.Append(std::move(material), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a MeshPrimitive. Unlike most types in glTF, MeshPrimitives are direct children
    // of their parent Mesh entity rather than being children of the Document. This is why they
    // don't have an ID member.
    Microsoft::glTF::MeshPrimitive meshPrimitive;
    meshPrimitive.materialId = materialId;
    meshPrimitive.indicesAccessorId = accessorIdIndices;
    meshPrimitive.attributes[Microsoft::glTF::ACCESSOR_POSITION] = accessorIdPositions;

    // Construct a Mesh and add the MeshPrimitive as a child
    Microsoft::glTF::Mesh mesh;
    mesh.primitives.push_back(std::move(meshPrimitive));
    // Add it to the Document and store the generated ID
    auto meshId = document.meshes.Append(std::move(mesh), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Node adding a reference to the Mesh
    Microsoft::glTF::Node node;
    node.meshId = meshId;
    // Add it to the Document and store the generated ID
    auto nodeId = document.nodes.Append(std::move(node), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty).id;

    // Construct a Scene
    Microsoft::glTF::Scene scene;
    scene.nodes.push_back(nodeId);
    // Add it to the Document, using a utility method that also sets the Scene as the Document's default
    document.SetDefaultScene(std::move(scene), Microsoft::glTF::AppendIdPolicy::GenerateOnEmpty);
}

void LandXMLModel2glTF::WriteGLTFFile(Microsoft::glTF::Document& document, std::filesystem::path& glTFFilename)
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
    std::string accessorIdIndices, accessorIdPositions;

    AddGLTFMeshBuffers(document, bufferBuilder, accessorIdIndices, accessorIdPositions);

    AddGLTFMesh(document, accessorIdIndices, accessorIdPositions);

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

