#pragma once

#include "framework.h"
#include "GLTF.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

class LANDXML2GLTFDLLAPI LandXMLPoint3D
{
public:
    inline LandXMLPoint3D() {};
    inline LandXMLPoint3D(double inX, double inY, double inZ)
    {
        x = inX;
        y = inY;
        z = inZ;
    };

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};


class LANDXML2GLTFDLLAPI LandXMLMaterial
{
public:
    std::string m_name;
    int m_ID = 0;
    std::string m_description;
    std::string m_RGBColorStr;
    std::string m_layerName;
    std::string m_textureName;
    std::string m_lineType;
    std::string m_textureImageRef;
    double m_textureImageScale = 1.0;
    std::string m_textureImageWorldFile;
    std::string m_textureImageWKT;
    std::string m_symbolReferenceStr;
    double m_symbolXScale = 1.0;
    double m_symbolYScale = 1.0;
    double m_symbolZScale = 1.0;
    double m_symbolRotation = 0.0;
    char* m_textureImageHexString = nullptr;
};

class LANDXML2GLTFDLLAPI LandXMLMaterialTable
{
public:
    std::unordered_map<int, LandXMLMaterial> m_MaterialMap;
};

class LANDXML2GLTFDLLAPI LandXMLPolyline
{
public:
    std::string m_name;
    int m_materialID = 0;
    std::string m_description;
    std::vector<LandXMLPoint3D> m_polylinePoints;
    double m_area = 0.0;
};

class LANDXML2GLTFDLLAPI LandXMLSurfaceFace
{
public:
    std::vector<unsigned int> m_pointIndices;
};

class LANDXML2GLTFDLLAPI LandXMLSurfaceMesh
{
public:
    int m_materialID = 0;
    std::string m_materialName;

    std::vector<LandXMLSurfaceFace> m_surfaceFaces;
};

class LANDXML2GLTFDLLAPI LandXMLSurface
{
public:
    std::string m_name;
    std::string m_description;

    std::vector<LandXMLPoint3D> m_surfacePoints;
    std::list<LandXMLPolyline> m_textureBoundaries;
    std::unordered_map<int,LandXMLSurfaceMesh*> m_surfaceMeshes;
};

#ifdef _MSC_VER
#   pragma warning(pop)
#endif