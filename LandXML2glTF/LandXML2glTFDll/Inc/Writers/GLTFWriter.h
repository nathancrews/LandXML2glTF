#pragma once

#include "framework.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <GLTF.h>
#include <IStreamWriter.h>
#include <BufferBuilder.h>
#include <GLTFResourceWriter.h>
#include <GLBResourceWriter.h>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

class LANDXML2GLTFDLLAPI StreamWriter : public Microsoft::glTF::IStreamWriter
{
public:
    StreamWriter(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
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
        auto streamPath = m_pathBase / std::filesystem::u8path(filename);
        auto stream = std::make_shared<std::ofstream>(streamPath, std::ios_base::binary);

        // Check if the stream has no errors and is ready for I/O operations
        if (!stream || !(*stream))
        {
            throw std::runtime_error("Error creating output stream for uri: " + filename);
        }

        return stream;
    }

private:
    std::filesystem::path m_pathBase;
};

class LANDXML2GLTFDLLAPI GLTFWriter
{
public:

    void WriteGLTFFile(std::filesystem::path& glTFFilename);



};

#ifdef _MSC_VER
#   pragma warning(pop)
#endif