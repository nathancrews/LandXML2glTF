#pragma once

#include "framework.h"
#include "LandXML.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cmath>
#include "GLTF.h"
#include <IStreamWriter.h>
#include "tinyxml2.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 
#include <experimental/filesystem>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

class OGRCoordinateTransformation;

namespace LANDXML2GLTF
{

    class LANDXML2GLTFDLLAPI LandXMLModel2glTF
    {
    public:

        bool Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename);
        void WriteGLTFFile(std::experimental::filesystem::path& glTFFilename);

        // helper function to find LandXML Surface triangle texture materials
        bool PointInPolygon(Microsoft::glTF::Vector3& point, std::vector<Microsoft::glTF::Vector3>& polygonPoints);

    private:

        OGRCoordinateTransformation* m_wgsTrans = nullptr;
        tinyxml2::XMLDocument* m_LXDocument = nullptr;
        LandXMLModel m_landXMLModel;
    };

}

class StreamWriter : public Microsoft::glTF::IStreamWriter
{
public:
    StreamWriter(std::experimental::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
    {
        assert(m_pathBase.has_root_path());
    }

    // Resolves the relative URIs of any external resources declared in the glTF manifest
    std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const
    {
        // In order to construct a valid stream:
        // 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
        //    correctly construct a path instance.
        // 2. Generate an absolute path by concatenating m_pathBase with the specified filename
        //    path. The filesystem::operator/ uses the platform's preferred directory separator
        //    if appropriate.
        // 3. Always open the file stream in binary mode. The glTF SDK will handle any text
        //    encoding issues for us.
        auto streamPath = m_pathBase / std::experimental::filesystem::u8path(filename);
        auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

        // Check if the stream has no errors and is ready for I/O operations
        if (!stream || !(*stream))
        {
            throw std::runtime_error("Error creating output stream for uri: " + filename);
        }

        return stream;
    }

private:
    std::experimental::filesystem::path m_pathBase;
};

#ifdef _MSC_VER
#   pragma warning(pop)
#endif