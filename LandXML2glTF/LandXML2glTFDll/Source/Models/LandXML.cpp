#include "Models/LandXML.h"
#include "Helpers/MathHelper.h"

namespace LANDXML2GLTF
{

LandXMLPoint3D::LandXMLPoint3D(const double inX, const double inY, const double inZ)
{
    x = inX;
    y = inY;
    z = inZ;
};

bool LandXMLPoint3D::operator<(const LandXMLPoint3D& vec)
{
    return ((x < vec.x) || (y < vec.y) || (z < vec.z));
}

bool LandXMLPoint3D::operator>(const LandXMLPoint3D& vec)
{
    return ((x > vec.x) || (y > vec.y) || (z > vec.z));
}

bool LandXMLPoint3D::operator==(const LandXMLPoint3D& vec) const 
{
    return (MathHelper::IsFuzzyEqual(x, vec.x) && MathHelper::IsFuzzyEqual(y, vec.y) && MathHelper::IsFuzzyEqual(z, vec.z));
};

}