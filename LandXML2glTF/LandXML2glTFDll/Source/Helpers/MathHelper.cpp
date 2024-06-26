////////////////////////////////////////////////////////////////////////////////////
// Copyright 2023-2024 Nathan C. Crews IV
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////
#include "Helpers/MathHelper.h"

namespace LANDXML2GLTF
{

bool MathHelper::PointInPolygon(const LandXMLPoint3D& point, const std::vector<LandXMLPoint3D>& polygonPoints)
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

double MathHelper::GetPolygonArea(const std::vector<LandXMLPoint3D>& polygonPoints)
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

#ifdef USE_GDAL
OGRCoordinateTransformation* MathHelper::GetWGS84CoordTransform(OGRSpatialReference& LXCoordRef)
{
    OGRSpatialReference wgs84Ref;
    OGRErr retStat;

    retStat = wgs84Ref.SetWellKnownGeogCS("WGS84");

    OGRCoordinateTransformation* wgsTrans = OGRCreateCoordinateTransformation(&LXCoordRef, &wgs84Ref);

    return wgsTrans;
}
#endif

double MathHelper::GetDistance2D(const LandXMLPoint3D& fromPoint, const LandXMLPoint3D& toPoint)
{
    return std::sqrt(std::pow((toPoint.x - fromPoint.x), 2) + std::pow((toPoint.y - fromPoint.y), 2));
}

double MathHelper::GetAngle2D(const LandXMLPoint3D& fromPoint, const LandXMLPoint3D& toPoint)
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

 bool MathHelper::Tesselate2DCurve(const LandXMLPoint3D& startPnt, const LandXMLPoint3D& centerPoint, const LandXMLPoint3D& endPnt, 
                                   std::vector<LandXMLPoint3D>& OutTesselatedPointList)
{
    unsigned int TessalationSegments = 8;
    double theta = 0.0;
    double delta = 0.0;
    double radius = MathHelper::GetDistance2D(startPnt, centerPoint);
    double startAng = MathHelper::GetAngle2D(centerPoint, startPnt);
    double endAng = MathHelper::GetAngle2D(centerPoint, endPnt);

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

    for (unsigned int i = 0; i < TessalationSegments; i++)
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

