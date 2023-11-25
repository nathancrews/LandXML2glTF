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

    area = std::abs(area) / 2.0;

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

double MathHelper::Distance2D(LandXMLPoint3D& fromPoint, LandXMLPoint3D& toPoint)
{
    return std::sqrt(std::pow((toPoint.x - fromPoint.x), 2) + std::pow((toPoint.y - fromPoint.y), 2));
}

double MathHelper::Angle2D(LandXMLPoint3D& fromPoint, LandXMLPoint3D& toPoint)
{
    double angle = 0.0;

    if (fromPoint == toPoint)
    {
        return angle;
    }
    // 90 or 270
    else if (IsFuzzyEqual(fromPoint.x, toPoint.x))
    {
        angle = PI / 2.0;

        if (fromPoint.y > toPoint.y)
        {
            angle += PI;
        }
    }
    // 0 or 180
    else if (IsFuzzyEqual(fromPoint.y, toPoint.y))
    {
        angle = 0.0;

        if (fromPoint.x > toPoint.x)
        {
            angle += PI;
        }
    }
    else
    {
        // Quad 1
        angle = std::atan(std::abs((toPoint.y - fromPoint.y) / (toPoint.x - fromPoint.x)));

        // Quad 2
        if (fromPoint.x > toPoint.x && fromPoint.y < toPoint.y)
        {
            angle = PI - angle;
        }
        // Quad 3
        else if (fromPoint.x > toPoint.x && fromPoint.y > toPoint.y)
        {
            angle += PI;
        }
        // Quad 4
        else if (fromPoint.x < toPoint.x && fromPoint.y > toPoint.y)
        {
            angle = 2.0 * PI - angle;
        }
    }

    return angle;
}

 bool MathHelper::Tesselate2DCurve(LandXMLPoint3D& startPnt, LandXMLPoint3D& centerPoint, LandXMLPoint3D& endPnt, std::vector<LandXMLPoint3D>& OutTesselatedPointList)
{
    unsigned int TessalationSegments = 8;
    double theta = 0.0;
    double delta = 0.0;
    double radius = MathHelper::Distance2D(startPnt, centerPoint);
    double startAng = MathHelper::Angle2D(centerPoint, startPnt);
    double endAng = MathHelper::Angle2D(centerPoint, endPnt);

    if (endAng < (PI / 2.0) && startAng > PI)
    {
        endAng += PI * 2;
    }
    else if (endAng > PI && startAng < (PI / 2.0))
    {
        startAng += PI * 2;
    }

    theta = startAng;
    delta = (endAng - startAng) / TessalationSegments;

    for (int i = 0; i < TessalationSegments; i++)
    {
        LandXMLPoint3D tessPnt;

        tessPnt.x = centerPoint.x + (radius * cos(theta));
        tessPnt.y = centerPoint.y + (radius * sin(theta));
        tessPnt.z = 0.0;

        OutTesselatedPointList.push_back(tessPnt);
        theta += delta;
    }

    return (OutTesselatedPointList.size() > 0);
}

}

