#include "framework.h"
#include "LandXMLModel2glTF.h"
#include "LandXML2glTFDLL.h"

#ifdef USE_GDAL
#pragma warning(push)
#pragma warning(disable: 4005)
#include "gdal.h"
#include "ogr_spatialref.h"
#include "cpl_conv.h"
#pragma warning(pop)
#endif

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
#ifdef USE_GDAL
    CPLSetConfigOption("GDAL_DATA", inAppDataPath.c_str());
    CPLSetConfigOption("PROJ_LIB", inAppDataPath.c_str());
#endif

    LANDXML2GLTF::LandXMLModel2glTF LXMLModel;

    return LXMLModel.Convert2glTFModel(inLandXMLFilename, glTFFilename);
}

