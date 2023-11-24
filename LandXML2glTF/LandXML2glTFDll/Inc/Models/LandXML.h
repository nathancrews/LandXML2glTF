#pragma once

#include "framework.h"
#include "GLTF.h"
#include <map>

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

    std::string m_name;
    std::string m_description;
    std::string m_code;
    unsigned int m_materialID = 1;

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct LANDXML2GLTFDLLAPI LandXMLMaterial
{
    unsigned int m_ID = 1;
    double m_textureImageScale = 1.0;
    double m_symbolXScale = 1.0;
    double m_symbolYScale = 1.0;
    double m_symbolZScale = 1.0;
    double m_symbolRotation = 0.0;
    std::string m_name;
    std::string m_description;
    std::string m_RGBColorStr = "0,255,0";
    std::string m_layerName;
    std::string m_textureName;
    std::string m_lineType;
    std::string m_textureImageRef;
    std::string m_textureImageWorldFile;
    std::string m_textureImageWKT;
    std::string m_symbolReferenceStr;
    std::string m_textureImageHexString;
};

struct LANDXML2GLTFDLLAPI LandXMLMaterialTable
{
    std::unordered_map<int, LandXMLMaterial> m_MaterialMap;
};

struct LANDXML2GLTFDLLAPI LandXMLPolyline
{
    unsigned int m_materialID = 1;
    double m_area = 0.0;

    std::string m_name;
    std::string m_description;
    std::vector<LandXMLPoint3D> m_polylinePoints;
};

struct LANDXML2GLTFDLLAPI LandXMLAlignment : public LandXMLPolyline
{

};

struct LANDXML2GLTFDLLAPI LandXMLParcel : public LandXMLPolyline
{

};

struct LANDXML2GLTFDLLAPI LandXMLSurfaceFace
{
    std::vector<UINT> m_pointIndices;
};

struct LANDXML2GLTFDLLAPI LandXMLTextureBoundary : public LandXMLPolyline
{
    std::vector<LandXMLSurfaceFace> m_surfaceFaces;
};

struct LANDXML2GLTFDLLAPI LandXMLSurface
{
    LandXMLSurface()
    {
        m_minSurfPoint.x = DBL_MAX;
        m_minSurfPoint.y = DBL_MAX;
        m_minSurfPoint.z = DBL_MAX;

        m_maxSurfPoint.x = -DBL_MAX;
        m_maxSurfPoint.y = -DBL_MAX;
        m_maxSurfPoint.z = -DBL_MAX;
    }

    ~LandXMLSurface(){}

    std::string m_name;
    std::string m_description;

    LandXMLPoint3D m_minSurfPoint;
    LandXMLPoint3D m_maxSurfPoint;

    unsigned int m_surfacePointCount = 0;
    std::map<unsigned int, LandXMLPoint3D> m_surfacePoints;
    std::list<LandXMLTextureBoundary> m_textureBoundaries;
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

    std::vector<LandXMLPolyline> m_landxmlPlanFeatures;
    std::vector<LandXMLAlignment> m_landxmlAlignments;
    std::vector<LandXMLParcel> m_landxmlParcels;
    std::map<std::string, LandXMLPoint3D> m_landxmlPoints;
};


#ifdef _MSC_VER
#   pragma warning(pop)
#endif