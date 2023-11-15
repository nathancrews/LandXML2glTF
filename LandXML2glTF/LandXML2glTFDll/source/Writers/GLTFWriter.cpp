#include "Writers/GLTFWriter.h"

using namespace Microsoft::glTF;

void GLTFWriter::WriteGLTFFile(std::filesystem::path& glTFFilename)
{
    Document originalDoc;
    Mesh gltfSurface;

    gltfSurface.primitives[0].mode = MeshMode::MESH_TRIANGLES;

    Scene sc; sc.id = "0";
    sc.nodes = { "0" };
    originalDoc.SetDefaultScene(std::move(sc));
    std::array<float, 16> matrixData; std::fill(matrixData.begin(), matrixData.end(), 1.0f);
    Matrix4 mat4; mat4.values = matrixData;
    Node matrixNode; matrixNode.id = "0"; matrixNode.name = "matrixNode";
    matrixNode.matrix = mat4;
    originalDoc.nodes.Append(std::move(matrixNode));
    auto outputString = Serialize(originalDoc);
    auto twoPassDoc = Deserialize(outputString);

   

    Microsoft::glTF::Buffer* LXBuffer = new  Microsoft::glTF::Buffer();

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

    //// If the file has a '.glb' extension then create a GLBResourceWriter. This class derives
    //// from GLTFResourceWriter and adds support for writing manifests to a GLB container's
    //// JSON chunk and resource data to the binary chunk.
    if (pathFileExt == MakePathExt(Microsoft::glTF::GLB_EXTENSION))
    {
        resourceWriter = std::make_unique<Microsoft::glTF::GLBResourceWriter>(std::move(streamWriter));
    }

    if (!resourceWriter)
    {
        throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
    }

    // The Document instance represents the glTF JSON manifest
    Microsoft::glTF::Document document;

    std::string accessorIdIndices;
    std::string accessorIdPositions;

    // Use the BufferBuilder helper class to simplify the process of
    // constructing valid glTF Buffer, BufferView and Accessor entities
    Microsoft::glTF::BufferBuilder bufferBuilder(std::move(resourceWriter));


    delete LXBuffer;
}