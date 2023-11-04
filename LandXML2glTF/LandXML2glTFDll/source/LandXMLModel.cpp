#include "LandXMLModel.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "TinyXML/Inc/tinyxml2.h"
#include "GLTFSDK/Inc/GLTF.h"

void LandXMLModel::run()
{

}



//bool PointInPolygon(Point point, Polygon polygon) {
//    vector<Point> points = polygon.getPoints();
//    int i, j, nvert = points.size();
//    bool c = false;
//
//    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
//        if (((points[i].y >= point.y) != (points[j].y >= point.y)) &&
//            (point.x <= (points[j].x - points[i].x) * (point.y - points[i].y) / (points[j].y - points[i].y) + points[i].x)
//            )
//            c = !c;
//    }
//
//    return c;
//}