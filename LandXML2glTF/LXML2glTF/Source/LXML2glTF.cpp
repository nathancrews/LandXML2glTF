#include "LandXML2glTFDll.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
    // LandXML-2.0 testing with surface texture materials
    std::filesystem::path LandXMLFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.xml";
    std::filesystem::path glTFFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.gltf";

    // LandXML-1.0 testing without surface texture materials
    //std::filesystem::path LandXMLFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\subdivision.xml";
    //std::filesystem::path glTFFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\subdivision.gltf";

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
        glTFFilename.replace_extension("glTF");
    }

    if (std::filesystem::exists(LandXMLFilename))
    {
        std::cout << "Converting " << LandXMLFilename << " to " << glTFFilename;

        LandXMLModel2GLTFDLL LandXML2glTFConverter;

        LandXML2glTFConverter.ConvertFile(LandXMLFilename.string(), glTFFilename.string());
    }
}


