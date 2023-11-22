#include "LandXML2glTFDll.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{
    std::filesystem::path LandXMLFilename;
    std::filesystem::path glTFFilename;
    std::filesystem::path gdalDataPath;

    bool doSubDirectories = false;
    std::string gltfExt = "gltf";

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

    for (int u = 1; u < argc; u++)
    {
        // convert to .glb file
        if (!_stricmp(argv[u], "/b"))
        {
            gltfExt = "glb";
        }
        
        // Recursively search sub-directories
        if (!_stricmp(argv[u], "/s"))
        {
            doSubDirectories = true;
        }
    }

    std::filesystem::path LandXMLFilenameABS = std::filesystem::absolute(LandXMLFilename);

    if (glTFFilename.empty())
    {
        glTFFilename = LandXMLFilenameABS;
        glTFFilename.replace_extension(gltfExt);
    }

    std::vector< std::filesystem::path> batchModeXMLFilenames;

    // Check if a batch mode is requested
    if (strstr(LandXMLFilenameABS.string().c_str(), "*"))
    {
        std::filesystem::path searchPath = LandXMLFilenameABS;

        searchPath._Remove_filename_and_separator();

        if (doSubDirectories)
        {
            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(searchPath))
            {
                if (dir_entry.is_regular_file() && !dir_entry.path().extension().compare(".xml"))
                {
                    batchModeXMLFilenames.push_back(dir_entry.path());
                }
            }
        }
        else
        {
            for (auto const& dir_entry : std::filesystem::directory_iterator(searchPath))
            {
                if (dir_entry.is_regular_file() && !dir_entry.path().extension().compare(".xml"))
                {
                    batchModeXMLFilenames.push_back(dir_entry.path());
                }
            }
        }
    }
    else
    {
        batchModeXMLFilenames.push_back(LandXMLFilenameABS);
    }

    int fcount = 1;

    LandXMLModel2GLTFDLL LandXML2glTFConverter;

    for (std::filesystem::path localLandXMLFilename : batchModeXMLFilenames)
    {
        if (std::filesystem::exists(localLandXMLFilename))
        {
            glTFFilename = localLandXMLFilename;
            glTFFilename.replace_extension(gltfExt);

            std::cout << "Converting " "[" << fcount << " of " << batchModeXMLFilenames.size() << "] " << localLandXMLFilename << " to " << glTFFilename << "\n";

            LandXML2glTFConverter.ConvertFile(localLandXMLFilename.string(), glTFFilename.string(), gdalDataPath.string());
        }
        else
        {
            std::cout << "Error opening: " << localLandXMLFilename << "\n" << "Conversion failed!";
        }

        fcount++;
    }
    }


