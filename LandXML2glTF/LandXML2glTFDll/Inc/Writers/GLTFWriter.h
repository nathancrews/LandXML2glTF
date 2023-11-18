#pragma once

#include "framework.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <GLTF.h>
#include <Document.h>
#include <Serialize.h>
#include <Deserialize.h>
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


    std::shared_ptr<std::ostream> GetOutputStream(const std::string& filename) const
    {
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