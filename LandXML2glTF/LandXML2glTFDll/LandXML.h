#pragma once

#include "framework.h"
#include "GLTF.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

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
    std::string m_textureImageScale;
    std::string m_textureImageWorldFile;
    std::string m_textureImageWKT;
    std::string m_symbolReferenceStr;
    double m_symbolXScale;
    double m_symbolYScale;
    double m_symbolZScale;
    double m_symbolRotation;
};

class LANDXML2GLTFDLLAPI LandXMLPolyline
{
public:
    std::string m_name;
    int m_materialID = 0;
    std::string m_description;
    std::vector<Microsoft::glTF::Vector3> m_polylinePoints;
};

class LANDXML2GLTFDLLAPI LandXMLSurfaceFace
{
public:
    std::vector<unsigned int> m_pointIndex;
};

class LANDXML2GLTFDLLAPI LandXMLSurface
{
public:
    std::string m_name;
    std::string m_description;

    std::vector<LandXMLPolyline> m_textureBoundaries;
    std::vector<Microsoft::glTF::Vector3> m_surfacePoints;
    std::vector<LandXMLSurfaceFace> m_surfaceFaces;
};

#ifdef _MSC_VER
#   pragma warning(pop)
#endif