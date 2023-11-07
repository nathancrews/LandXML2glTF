#include "LandXML2glTFDll.h"

int main()
{
    // LandXML-2.0 testing with surface texture materials
    //std::string LandXMLFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.xml";
    //std::string glTFFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\KYRoad.gltf";

    // LandXML-1.0 testing without surface texture materials
    std::string LandXMLFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\subdivision.xml";
    std::string glTFFilename = "D:\\GitHub\\LandXML2glTF\\LandXML\\subdivision.gltf";



    LandXMLModel2GLTF::ConvertFile(LandXMLFilename, glTFFilename);
}


