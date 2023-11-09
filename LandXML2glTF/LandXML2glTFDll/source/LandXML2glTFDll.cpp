#include "framework.h"
#include "LandXMLModel2glTF.h"
#include "LandXML2glTFDLL.h"

using namespace LANDXML2GLTF;

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

bool LandXMLModel2GLTFDLL::ConvertFile(const std::string& inLandXMLFilename, const std::string& glTFFilename, const std::string& inAppDataPath)
{
    LandXMLModel2glTF LXMLModel;

    return LXMLModel.Convert2glTFModel(inLandXMLFilename, glTFFilename);
}

