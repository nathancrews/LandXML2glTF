#pragma once

#include "framework.h"
#include "LandXML.h"

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4251)
#endif

namespace LANDXML2GLTF
{

    class LANDXML2GLTFDLLAPI MathHelper
    {
    public:

        static bool PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints);
        static double PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints);

    };

}

#ifdef _MSC_VER
#   pragma warning(pop)
#endif
