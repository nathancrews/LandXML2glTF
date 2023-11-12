#pragma once

#include "framework.h"
#include "GLTF.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

struct LANDXML2GLTFDLLAPI LandXMLPoint3D
{
    inline LandXMLPoint3D() {};
    inline LandXMLPoint3D(double inX, double inY, double inZ)
    {
        x = inX;
        y = inY;
        z = inZ;
    };

    inline void operator=(const LandXMLPoint3D& vec) { x = vec.x; y = vec.y; z = vec.z; };
 
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct LANDXML2GLTFDLLAPI LandXMLMaterial
{
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

struct LANDXML2GLTFDLLAPI LandXMLMaterialTable
{
    std::unordered_map<int, LandXMLMaterial> m_MaterialMap;
};

struct LANDXML2GLTFDLLAPI LandXMLPolyline
{
    std::string m_name;
    int m_materialID = 0;
    std::string m_description;
    std::vector<LandXMLPoint3D> m_polylinePoints;
    double m_area = 0.0;
};

struct LANDXML2GLTFDLLAPI LandXMLSurfaceFace
{
    std::vector<unsigned int> m_pointIndices;
};

struct LANDXML2GLTFDLLAPI LandXMLSurfaceMesh
{
    int m_materialID = 0;
    std::string m_materialName;

    std::vector<LandXMLSurfaceFace> m_surfaceFaces;
};

struct LANDXML2GLTFDLLAPI LandXMLSurface
{
    ~LandXMLSurface()
    {
        for (int j = 0; j < m_surfaceMeshes.size(); j++)
        {
            delete m_surfaceMeshes[j];
        }
    }

    std::string m_name;
    std::string m_description;

    std::vector<LandXMLPoint3D> m_surfacePoints;
    std::list<LandXMLPolyline> m_textureBoundaries;
    std::unordered_map<int,LandXMLSurfaceMesh*> m_surfaceMeshes;
};

struct LANDXML2GLTFDLLAPI LandXMLUnits
{
    std::string m_linearUnitString;
};

struct LANDXML2GLTFDLLAPI LandXMLModel
{
    ~LandXMLModel() 
    {
        for (int j = 0; j < m_landxmlSurfaces.size(); j++)
        {
            delete m_landxmlSurfaces[j];
        }
    }

    std::string m_fileName;
    std::string m_wktString;

    LandXMLUnits m_units;
    LandXMLMaterialTable m_landXMLMaterials;
    std::vector<LandXMLSurface*> m_landxmlSurfaces;
};


#ifdef _MSC_VER
#   pragma warning(pop)
#endif