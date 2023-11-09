
#include "LandXMLModel2glTF.h"
#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"
#include <BufferBuilder.h>
#include <GLTFResourceWriter.h>
#include <GLBResourceWriter.h>


namespace LANDXML2GLTF
{

    bool LandXMLModel2glTF::Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename)
    {
        bool retval = false;

        if (false == std::experimental::filesystem::exists(InLandXMLFilename))
        {
            return retval;
        }

        m_LXDocument = new XMLDocument(true, COLLAPSE_WHITESPACE);

        if (!m_LXDocument)
        {
            return retval;
        }

        m_LXDocument->LoadFile(InLandXMLFilename.c_str());

        LXParser LXHelper;
        LXHelper.ParseLandXMLFile(m_LXDocument, m_landXMLModel);

        m_wgsTrans = MathHelper::GetOGRCoordTransform(m_landXMLModel.m_wktString);

        // cleanup memory
        delete m_LXDocument;
        delete m_wgsTrans;

        return retval;
    }

    void LandXMLModel2glTF::WriteGLTFFile(std::experimental::filesystem::path& glTFFilename)
    {
        Microsoft::glTF::Buffer* LXBuffer = new  Microsoft::glTF::Buffer();

        // Pass the absolute path, without the filename, to the stream writer
        auto streamWriter = std::make_unique<StreamWriter>(glTFFilename.parent_path());

        std::experimental::filesystem::path pathFile = glTFFilename.filename();
        std::experimental::filesystem::path pathFileExt = pathFile.extension();

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

    bool LandXMLModel2glTF::PointInPolygon(Microsoft::glTF::Vector3& point, std::vector<Microsoft::glTF::Vector3>& polygonPoints)
    {
        size_t i = 0, j = 0, nvert = polygonPoints.size();
        bool PointInside = false;

        for (i = 0, j = nvert - 1; i < nvert; j = i++)
        {
            if (((polygonPoints[i].y >= point.y) != (polygonPoints[j].y >= point.y)) &&
                (point.x <= (polygonPoints[j].x - polygonPoints[i].x) * (point.y - polygonPoints[i].y) / (polygonPoints[j].y - polygonPoints[i].y) + polygonPoints[i].x))
            {
                PointInside = !PointInside;
            }
        }

        return PointInside;
    }
}