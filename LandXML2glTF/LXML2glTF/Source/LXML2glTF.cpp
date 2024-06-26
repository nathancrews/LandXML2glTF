////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Nathan C. Crews IV
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////
#include "LandXML2glTFDll.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[])
{

#ifdef _WIN32
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

    std::filesystem::path LandXMLFilename;
    std::filesystem::path glTFFilename;
    std::filesystem::path exeDataPath;

    bool doSubDirectories = false;
    std::string gltfExt = "gltf";

    if (argc > 0)
    {
        // gdal needs this
        std::filesystem::path exePath = argv[0];
        exeDataPath = exePath.remove_filename();

        exeDataPath.append("data");
    }

    if (argc > 1)
    {
        LandXMLFilename = argv[1];
    }

    if (LandXMLFilename.empty())
    {
            std::cout << "Error: No .xml filename or path specified." << std::endl;
            return 1;
    }

    for (int u = 1; u < argc; u++)
    {
        // convert to .glb file
        if (!stricmp(argv[u], "/b") || !strcmp(argv[u], "/B"))
        {
            gltfExt = "glb";
        }
        
        // Recursively search sub-directories
        if (!stricmp(argv[u], "/s") || !strcmp(argv[u], "/S"))
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

        searchPath.remove_filename();

        if (std::filesystem::exists(searchPath))
        {
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
            std::cout << "Error: path not found: " << searchPath << "\n";
            return 1;
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

            LandXML2glTFConverter.ConvertFile(localLandXMLFilename.string(), glTFFilename.string(), exeDataPath.string());
        }
        else
        {
            std::cout << "Error opening: " << localLandXMLFilename << "\n" << "Conversion failed!";
        }

        fcount++;
    }

        return 0;
    }


