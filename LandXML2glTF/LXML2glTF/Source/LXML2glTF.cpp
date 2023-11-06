#include "LandXML2glTFDll.h"

int main()
{
    std::string LandXMLFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.xml";
    std::string glTFFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.gltf";

    LandXMLModel2GLTF::ConvertFile(LandXMLFilename, glTFFilename);
}


