#pragma once

#include "framework.h"
#include "Models/LandXML.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#include "gdal.h"
#include "ogr_spatialref.h"
#pragma warning(pop)

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

#ifndef USFT2M			// U.S. feet to meters
#	define USFT2M		0.3048006096012192
#endif

#ifndef IFT2M			// International feet to meters
#	define IFT2M		0.3048
#endif

class OGRCoordinateTransformation;

namespace LANDXML2GLTF
{

    class LANDXML2GLTFDLLAPI MathHelper
    {
    public:

        static bool PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints);
        static double PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints);
        static OGRCoordinateTransformation* GetWGS84CoordTransform(OGRSpatialReference& LXCoordRef);
    };

}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif
