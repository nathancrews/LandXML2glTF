#include "Helpers/MathHelper.h"
#include <cmath>

namespace LANDXML2GLTF
{

    bool MathHelper::PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints)
    {
        size_t i = 0, j = 0, numVertices = polygonPoints.size();
        bool PointInside = false;

        for (i = 0, j = numVertices - 1; i < numVertices; j = i++)
        {
            if (((polygonPoints[i].y >= point.y) != (polygonPoints[j].y >= point.y)) &&
                (point.x <= (polygonPoints[j].x - polygonPoints[i].x) * (point.y - polygonPoints[i].y) / (polygonPoints[j].y - polygonPoints[i].y) + polygonPoints[i].x))
            {
                PointInside = !PointInside;
            }
        }

        return PointInside;
    }

    double MathHelper::PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints)
    {
        double area = 0.0;
        size_t numVertices = polygonPoints.size();

        for (int i = 0; i < numVertices - 1; ++i)
        {
            area += polygonPoints[i].x * polygonPoints[i + 1].y - polygonPoints[i + 1].x * polygonPoints[i].y;
        }

        area += polygonPoints[numVertices - 1].x * polygonPoints[0].y - polygonPoints[0].x * polygonPoints[numVertices - 1].y;

        area = fabs(area) / 2.0;

        return area;
    }

    OGRCoordinateTransformation* MathHelper::GetWGS84CoordTransform(OGRSpatialReference& LXCoordRef)
    {
        OGRSpatialReference wgs84Ref;
        OGRErr retStat;

        retStat = wgs84Ref.SetWellKnownGeogCS("WGS84");

        OGRCoordinateTransformation* wgsTrans = OGRCreateCoordinateTransformation(&LXCoordRef, &wgs84Ref);

        return wgsTrans;
    }

}

