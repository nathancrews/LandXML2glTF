
#include "LandXMLModel2glTF.h"
#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"


namespace LANDXML2GLTF
{

    bool LandXMLModel2glTF::Convert2glTFModel(const std::string& InLandXMLFilename, const std::string& glTFFilename)
    {
        bool retval = false;

        if (false == std::filesystem::exists(InLandXMLFilename))
        {
            return retval;
        }

        m_LXDocument = new XMLDocument(true, COLLAPSE_WHITESPACE);

        if (!m_LXDocument)
        {
            return retval;
        }

        m_LXDocument->LoadFile(InLandXMLFilename.c_str());

        LXParser LXHelper;

        LXHelper.ParseLandXMLHeader(m_LXDocument, m_landXMLModel);

        m_landXMLSpatialRef = new OGRSpatialReference();

        OGRErr tStat = m_landXMLSpatialRef->importFromWkt(m_landXMLModel.m_wktString.c_str());

        m_wgsTrans = MathHelper::GetWGS84CoordTransform(*m_landXMLSpatialRef);

        LXHelper.ParseLandXMLFile(m_LXDocument, m_landXMLModel);

        LandXMLPoint3D testPnt = m_landXMLModel.m_landxmlSurfaces[0]->m_surfacePoints[1];

        char* units = nullptr;
        m_unitConversionToWG84 = m_landXMLSpatialRef->GetLinearUnits(&units);

        int success = 0;

        m_wgsTrans->Transform(1, &testPnt.x, &testPnt.y, &testPnt.z, &success);

        testPnt.z = testPnt.z * m_unitConversionToWG84;

        // cleanup memory
        delete m_LXDocument;

        return retval;
    }

    

    bool LandXMLModel2glTF::PointInPolygon(Microsoft::glTF::Vector3& point, std::vector<Microsoft::glTF::Vector3>& polygonPoints)
    {
        size_t i = 0, j = 0, nvert = polygonPoints.size();
        bool PointInside = false;

        for (i = 0, j = nvert - 1; i < nvert; j = i++)
        {
            if (((polygonPoints[i].y >= point.y) != (polygonPoints[j].y >= point.y)) &&
                (point.x <= (polygonPoints[j].x - polygonPoints[i].x) * (point.y - polygonPoints[i].y) / (polygonPoints[j].y - polygonPoints[i].y) + polygonPoints[i].x))
            {
                PointInside = !PointInside;
            }
        }

        return PointInside;
    }
}