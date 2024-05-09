#pragma once

#include "framework.h"
#include "Models/LandXML.h"
#include <cfloat>

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

namespace LANDXML2GLTF
{
#ifndef USFT2M			// U.S. feet to meters
#	define USFT2M		0.3048006096012192
#endif

#ifndef IFT2M			// International feet to meters
#	define IFT2M		0.3048
#endif

#define PI 3.1415926535897932384626433832795

class LANDXML2GLTFDLLAPI MathHelper
{
public:

    static bool PointInPolygon(const LandXMLPoint3D& point, const std::vector<LandXMLPoint3D>& polygonPoints);
    static double GetPolygonArea(const std::vector<LandXMLPoint3D>& polygonPoints);
    static double GetDistance2D(const LandXMLPoint3D& P1, const LandXMLPoint3D& P2);
    static double GetAngle2D(const LandXMLPoint3D& fromPoint, const LandXMLPoint3D& toPoint);
    static bool Tesselate2DCurve(const LandXMLPoint3D& startPnt, const LandXMLPoint3D& centerPoint, const LandXMLPoint3D& endPnt, 
                                 std::vector<LandXMLPoint3D>& OutTesselatedPointList);

    static bool IsFuzzyEqual(const double lhs, const double rhs, const double tol = 0.001)
    {
        if (std::abs(lhs - rhs) < tol)
        {
            return true;
        }
        return false;
    }

    static bool IsFuzzyEqual(const float lhs, const float rhs, const float tol = 0.001)
    {
        if (std::abs(lhs - rhs) < tol)
        {
            return true;
        }
        return false;
    }

    //**********************************************************************************************************
    // Swap the incoming LandXML YZ axis to match GLTF Y up, z forward axis orientation

    static inline void CopyLXTOGLTFPoint(const LandXMLPoint3D& LXPnt, std::vector<float>& glTFPnt)
    {
        if (glTFPnt.size() > 2)
        {
            glTFPnt[0] = LXPnt.x; glTFPnt[2] = LXPnt.y; glTFPnt[1] = LXPnt.z;
        }
    }

    static inline void CopyLXTOGLTFPoint(const LandXMLPoint3D& LXPnt, float& glTFPnt1, float& glTFPnt2, float& glTFPnt3)
    {
        glTFPnt1 = LXPnt.x; glTFPnt3 = LXPnt.y; glTFPnt2 = LXPnt.z;
    }

    static inline void CopyLXTOGLTFPoint(const LandXMLPoint3D& LXPnt, Microsoft::glTF::Vector3& glTFPnt)
    {
        glTFPnt.x = LXPnt.x; glTFPnt.z = LXPnt.y; glTFPnt.y = LXPnt.z;
    }
    //**********************************************************************************************************

#ifdef USE_GDAL
    static OGRCoordinateTransformation* GetWGS84CoordTransform(OGRSpatialReference& LXCoordRef);
#endif

};

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif
