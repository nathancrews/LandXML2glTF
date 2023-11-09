#pragma once

#include "framework.h"
#include "LandXML.h"

#pragma warning(push)
#pragma warning(disable: 4005)
#include "gdal.h"
#include "ogr_spatialref.h"
#pragma warning(pop)

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

class OGRCoordinateTransformation;

namespace LANDXML2GLTF
{

    class LANDXML2GLTFDLLAPI MathHelper
    {
    public:

        static bool PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints);
        static double PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints);
        static OGRCoordinateTransformation* GetOGRCoordTransform(std::string& inWktString);
    };

}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif
