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
        glTFFilename = std::filesystem::absolute(glTFFilename);
    }

    std::filesystem::path LandXMLFilenameABS = std::filesystem::absolute(LandXMLFilename);

    if (glTFFilename.empty())
    {
        glTFFilename = LandXMLFilenameABS;
        glTFFilename.replace_extension("gltf");
    }

    if (std::filesystem::exists(LandXMLFilenameABS))
    {
        std::cout << "Converting " << LandXMLFilename << " to " << glTFFilename << "\n";

        LandXMLModel2GLTFDLL LandXML2glTFConverter;

        LandXML2glTFConverter.ConvertFile(LandXMLFilenameABS.string(), glTFFilename.string(), gdalDataPath.string());
    }
    else
    {
        std::cout << "Error opening: " << LandXMLFilename << "\n" << "Conversion failed!";
    }

}


