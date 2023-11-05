#include "framework.h"
#include "LandXMLModel.h"
#include "LandXML2glTFDLL.h"

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

using namespace LANDXML2GLTF;

bool LandXMLModel2GLTF::ConvertFile(const std::string& InLandXMLFilename, const std::string& glTFFilename)
{
    LandXMLModel LXMLModel;

    return LXMLModel.Convert2glTFModel(InLandXMLFilename, glTFFilename);
}

