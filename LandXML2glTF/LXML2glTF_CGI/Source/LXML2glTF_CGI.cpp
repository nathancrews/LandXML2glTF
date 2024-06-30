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
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <fstream>

#pragma warning (push)
#pragma warning (disable: 4251)
#include "CgiUtils.h"
#include "CgiDefs.h"
#include "Cgicc.h"
#include "HTTPHTMLHeader.h"
#include "HTTPRedirectHeader.h"
#pragma warning (pop)

using namespace cgicc;

int main(int argc, char** argv, char** env)
{
#if 0
    std::string header = "Content-type: text/plain\n\n";
    std::cout << header;
#endif

    const std::string landxml_default_dir = "uploads/landxml";
    const std::string gltf_default_dir = "uploads/gltf";
    const std::string data_default_dir = "data";

    bool isPostRequest = false;

    std::filesystem::path doc_root, app_path, uploaded_file_name;
    std::string queryStr, fileStr;


    for (; *argv != nullptr; argv++)
    {
        Debug_log("arg: %s\n", *argv);
    }

    for (; *env != nullptr; env++)
    {
        std::string envStr = *env;
        Debug_log("env: %s\n", envStr.c_str());

        if (envStr.find("REQUEST_METHOD") == 0)
        {
            std::stringstream ss(envStr);
            std::string token;

            std::getline(ss, token, '=');
            Debug_log("found REQUEST_METHOD: %s\n", token.c_str());

            std::getline(ss, token, '=');
            Debug_log("REQUEST_METHOD value: %s\n", token.c_str());

            //if (token.compare("post"))
            //{
            //    isPostRequest = true;

            //    Cgicc cgi;
            //    const_file_iterator file = cgi.getFile("file");

            //    if (file != cgi.getFiles().end())
            //    {
            //        uploaded_file_name = doc_root;
            //        uploaded_file_name.append(landxml_default_dir);
            //        uploaded_file_name.append(file->getFilename());
            //        uploaded_file_name.make_preferred();

            //        Debug_log("File name to upload: %s\n", uploaded_file_name.string().c_str());

            //        std::fstream fs;
            //        fs.open(uploaded_file_name, std::fstream::out | std::fstream::binary);

            //        if (fs.is_open())
            //        {
            //            file->writeToStream(fs);

            //            fs.flush();
            //            fs.close();

            //            Debug_log("File uploaded Succeeded: %s\n", uploaded_file_name.string().c_str());
            //            fileStr = file->getFilename();
            //        }
            //        else
            //        {
            //            Debug_log("File upload failed: %s\n", uploaded_file_name.string().c_str());
            //            uploaded_file_name = "";
            //        }
            //    }
            //}
        }

        else if (envStr.find("DOCUMENT_ROOT") == 0)
        {
            std::stringstream ss(envStr);
            std::string token;

            std::getline(ss, token, '=');
            Debug_log("found DOCUMENT_ROOT: %s\n", token.c_str());

            std::getline(ss, token, '=');
            Debug_log("DOCUMENT_ROOT value: %s\n", token.c_str());

            doc_root = token;
        }

        else if (envStr.find("CONTEXT_DOCUMENT_ROOT") == 0)
        {
            std::stringstream ss(envStr);
            std::string token;

            std::getline(ss, token, '=');
            Debug_log("found CONTEXT_DOCUMENT_ROOT: %s\n", token.c_str());

            std::getline(ss, token, '=');
            Debug_log("CONTEXT_DOCUMENT_ROOT value: %s\n", token.c_str());

            app_path = token;
        }

        else if (envStr.find("QUERY_STRING") < envStr.size())
        {
            std::stringstream ss(envStr);
            std::string token;

            std::getline(ss, token, '=');
            Debug_log("found QUERY_STRING: %s\n", token.c_str());

            std::getline(ss, token, '=');
            Debug_log("QUERY_STRING value: %s\n", token.c_str());

            queryStr = token;

            if (queryStr.compare("file") == 0)
            {
                std::getline(ss, token, '=');

                fileStr = token;
                Debug_log("file value: %s\n", fileStr.c_str());
            }

        }

    }

    bool doSubDirectories = false;
    std::string gltfExt = "gltf";
    std::filesystem::path LandXMLFilename;
    std::filesystem::path glTFFilename;
    std::filesystem::path exeDataPath;

    if (uploaded_file_name.string().size() > 0)
    {
        LandXMLFilename = uploaded_file_name;
    }
    else
    {
        LandXMLFilename = doc_root;
        LandXMLFilename.append(landxml_default_dir);
        LandXMLFilename.append(fileStr);
    }

    LandXMLFilename.make_preferred();
    Debug_log("LandXMLFilename: %s\n", LandXMLFilename.string().c_str());

    // gdal needs this
    exeDataPath = app_path;
    exeDataPath.append(data_default_dir);
    exeDataPath.make_preferred();

    Debug_log("app data path: %s\n", exeDataPath.string().c_str());

    glTFFilename = doc_root;
    glTFFilename.append(gltf_default_dir);
    glTFFilename.append(fileStr);
    glTFFilename.replace_extension(gltfExt);
    glTFFilename.make_preferred();

    Debug_log("glTFFilename: %s\n", glTFFilename.string().c_str());


    if (std::filesystem::exists(glTFFilename))
    {
        Debug_log("Skipping GLTF Generation, file exists %s\n", glTFFilename.string().c_str());
    }
    else
    {

        LandXMLModel2GLTFDLL LandXML2glTFConverter;
        LandXML2glTFConverter.ConvertFile(LandXMLFilename.string(), glTFFilename.string(), exeDataPath.string());
    }

    if (std::filesystem::exists(glTFFilename))
    {
        std::filesystem::path outfileName = glTFFilename.filename();
        outfileName.replace_extension(gltfExt);

        std::string toURL = "ncimagegen_cgi.exe?file=" + outfileName.string();
        HTTPRedirectHeader myheader(toURL, false);
        std::cout << myheader << "\r\n\r\n";

        //std::string header = "Content-type: text/plain\n\n";
        //std::cout << header;

        //header = "Content-type: model/gltf+json\n\n";
        //std::cout << header;

        //std::cout << glTFFilename << "\n\n";
    }

    return 0;
}
