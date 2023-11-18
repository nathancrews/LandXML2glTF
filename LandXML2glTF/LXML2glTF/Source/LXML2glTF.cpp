#include "LandXML2glTFDll.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
    std::filesystem::path LandXMLFilename;
    std::filesystem::path glTFFilename;
    std::filesystem::path gdalDataPath;

    if (argc > 0)
    {
        // gdal needs this
        std::filesystem::path exePath = argv[0];
        gdalDataPath = exePath.remove_filename();

        gdalDataPath.append("data");
    }

    if (argc > 1)
    {
        LandXMLFilename = argv[1];
    }

    if (argc > 2)
    {
        glTFFilename = argv[2];
    }

    if (glTFFilename.empty())
    {
        glTFFilename = LandXMLFilename;
        glTFFilename.replace_extension("gltf");
    }

    if (std::filesystem::exists(LandXMLFilename))
    {
        std::cout << "Converting " << LandXMLFilename << " to " << glTFFilename;

        LandXMLModel2GLTFDLL LandXML2glTFConverter;

        LandXML2glTFConverter.ConvertFile(LandXMLFilename.string(), glTFFilename.string(), gdalDataPath.string());
    }
}


