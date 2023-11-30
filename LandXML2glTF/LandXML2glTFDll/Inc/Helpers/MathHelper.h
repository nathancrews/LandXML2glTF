#pragma once

#include "framework.h"
#include "Models/LandXML.h"


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4005)
#pragma warning(disable: 4251)
#endif

#ifdef USE_GDAL
#include "gdal.h"
#include "ogr_spatialref.h"

class OGRCoordinateTransformation;
#endif

#ifndef USFT2M			// U.S. feet to meters
#	define USFT2M		0.3048006096012192
#endif

#ifndef IFT2M			// International feet to meters
#	define IFT2M		0.3048
#endif

namespace LANDXML2GLTF
{

#define PI 3.1415926535897932384626433832795

class LANDXML2GLTFDLLAPI MathHelper
{
public:

    static bool PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints);
    static double PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints);
    static double Distance2D(LandXMLPoint3D& P1, LandXMLPoint3D& P2);
    static double Angle2D(LandXMLPoint3D& fromPoint, LandXMLPoint3D& toPoint);
    static bool Tesselate2DCurve(LandXMLPoint3D& startPnt, LandXMLPoint3D& centerPoint, LandXMLPoint3D& endPnt, std::vector<LandXMLPoint3D>& OutTesselatedPointList);

    static bool IsFuzzyEqual(double lhs, double rhs, double tol = 0.001)
    {
        if (std::abs(lhs - rhs) < tol)
        {
            return true;
        }
        return false;
    }

    static bool IsFuzzyEqual(float lhs, float rhs, float tol = 0.001)
    {
        if (std::abs(lhs - rhs) < tol)
        {
            return true;
        }
        return false;
    }

    static inline void CopyLXTOGLTFPoint(LandXMLPoint3D& LXPnt, std::vector<float>& glTFPnt)
    {
        if (glTFPnt.size() > 2)
        {
            glTFPnt[0] = LXPnt.x; glTFPnt[1] = LXPnt.y; glTFPnt[2] = LXPnt.z;
        }
    }

    static inline void CopyLXTOGLTFPoint(LandXMLPoint3D& LXPnt, float& glTFPnt1, float& glTFPnt2, float& glTFPnt3)
    {
        glTFPnt1 = LXPnt.x; glTFPnt2 = LXPnt.y; glTFPnt3 = LXPnt.z;
    }

    static inline void CopyLXTOGLTFPoint(LandXMLPoint3D& LXPnt, Microsoft::glTF::Vector3& glTFPnt)
    {
        glTFPnt.x = LXPnt.x; glTFPnt.y = LXPnt.y; glTFPnt.z = LXPnt.z;
    }

#ifdef USE_GDAL
    static OGRCoordinateTransformation* GetWGS84CoordTransform(OGRSpatialReference& LXCoordRef);
#endif

};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
