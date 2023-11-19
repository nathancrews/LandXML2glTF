#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"
#include <functional>
#include <iostream>
#include <fstream>


namespace LANDXML2GLTF
{
bool LXParser::ParseLandXMLHeader(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    if (!LXDocument || !LXDocument->RootElement())
    {
        return retStat;
    }

    XMLElement* LXRoot = LXDocument->RootElement();

    // Parse Units and coordinate system
    XMLElement* LXUnits = LXRoot->FirstChildElement("Units");

    // The Units element and linearUnit are REQUIRED for any meaningful conversion
    if (!LXUnits)
    {
        std::cout << "error: The LandXML file is missing the required <Units> element.";
        return retStat;
    }

    XMLElement* LXUnit = LXUnits->FirstChildElement();

    if (LXUnit)
    {
        const XMLAttribute* linearUnitAT = LXUnit->FindAttribute("linearUnit");

        if (!linearUnitAT)
        {
            std::cout << "error: The LandXML file is missing the required <Units> element attribute \"linearUnit\".";
            return retStat;
        }

        outLandXMLMDoc.m_units.m_linearUnitString = linearUnitAT->Value();

        if (outLandXMLMDoc.m_units.m_linearUnitString.empty())
        {
            std::cout << "error: The LandXML file is missing a value for the required <Units> element attribute \"linearUnit\".";
            return retStat;
        }
    }

    XMLElement* LXCoordSystem = LXRoot->FirstChildElement("CoordinateSystem");

    if (LXCoordSystem)
    {
        const XMLAttribute* cSAt = LXCoordSystem->FindAttribute("ogcWktCode");
        if (cSAt)
        {
            outLandXMLMDoc.m_wktString = cSAt->Value();
        }
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    if (!LXDocument || !LXDocument->RootElement())
    {
        return retStat;
    }

    XMLElement* LXRoot = LXDocument->RootElement();

    // Units and CorrdinateSystem are parsed in ParseLandXMLHeader()

    // Parse Material table
    XMLElement* LXMaterials = LXRoot->FirstChildElement("MaterialTable");

    // Not all LandXML files contain materials, so create a single texture material table to use
    // if no materials are loaded, then fail

    if (!ParseMaterialTable(LXMaterials, outLandXMLMDoc.m_landXMLMaterials))
    {
        std::cout << "error: Unable to find LandXML <MaterialTable> element in ./data/DefaultTexture.xml";
        return retStat;
    }

    // Parse Surfaces
    XMLElement* LXSurfaces = LXRoot->FirstChildElement("Surfaces");
    if (!LXSurfaces)
    {
        return retStat;
    }

    XMLElement* LXSurface = LXSurfaces->FirstChildElement("Surface");

    while (LXSurface)
    {
        LandXMLSurface* LXSurfaceModel = new LandXMLSurface();

        ParseSurface(LXSurface, outLandXMLMDoc.m_landXMLMaterials, *LXSurfaceModel);

        outLandXMLMDoc.m_landxmlSurfaces.push_back(LXSurfaceModel);

        LXSurface = LXSurface->NextSiblingElement("Surface");
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseMaterialTable(XMLElement* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials)
{
    bool retStat = false;
    tinyxml2::XMLDocument defaultMatDoc;
    XMLElement* defaultMatRootElem = nullptr;
    XMLElement* LXMaterialTableToUse = nullptr;

    // probably a LandXML 1.0 or 1.2 file, so load the default texture material from data file
    if (!LXMaterialsNode)
    {
        if (defaultMatDoc.LoadFile("./data/DefaultTexture.xml") == XML_SUCCESS)
        {
            defaultMatRootElem = defaultMatDoc.RootElement();

            if (defaultMatRootElem)
            {
                LXMaterialTableToUse = defaultMatRootElem->FirstChildElement("MaterialTable");
            }
        }
    }
    else
    {
        LXMaterialTableToUse = LXMaterialsNode;
    }

    if (!LXMaterialTableToUse)
    {
        return retStat;
    }

    XMLElement* LXMaterial = LXMaterialTableToUse->FirstChildElement("Material");
    XMLElement* LXTextureImageTable = LXMaterialTableToUse->NextSiblingElement("TextureImageTable");

    while (LXMaterial)
    {
        LandXMLMaterial LXMaterialEntry;

        const XMLAttribute* materialName = LXMaterial->FindAttribute("name");
        if (materialName)
        {
            LXMaterialEntry.m_name = materialName->Value();
        }

        const XMLAttribute* materialAt = LXMaterial->FindAttribute("index");
        if (materialAt)
        {
            LXMaterialEntry.m_ID = std::atoi(materialAt->Value());
        }

        const XMLAttribute* materialColor = LXMaterial->FindAttribute("color");
        if (materialColor)
        {
            LXMaterialEntry.m_RGBColorStr = materialColor->Value();
        }

        const XMLAttribute* textureImageRefName = LXMaterial->FindAttribute("textureImageRef");
        if (textureImageRefName)
        {
            LXMaterialEntry.m_textureImageRef = textureImageRefName->Value();
            LXMaterialEntry.m_textureName = LXMaterialEntry.m_textureImageRef;

            if (LXMaterialEntry.m_name.empty())
            {
                LXMaterialEntry.m_name = LXMaterialEntry.m_textureName;
            }

            if (LXTextureImageTable)
            {
                XMLElement* textureImage = LXTextureImageTable->FirstChildElement("TextureImage");

                while (textureImage)
                {
                    const XMLAttribute* textureNameAt = textureImage->FindAttribute("name");

                    if (textureNameAt && !LXMaterialEntry.m_textureName.compare(textureNameAt->Value()))
                    {
                        XMLNode* TextureHexString = textureImage->FirstChildElement("TextureHexString");

                        if (TextureHexString)
                        {
                            size_t hexStrLen = strlen(TextureHexString->FirstChild()->Value()) + 1;
                            LXMaterialEntry.m_textureImageHexString = TextureHexString->FirstChild()->Value();
                        }

                        break;
                    }

                    textureImage = textureImage->NextSiblingElement("TextureImage");
                }

            }

        }

        const XMLAttribute* textureImageScaleAt = LXMaterial->FindAttribute("textureImageScale");
        if (textureImageScaleAt)
        {
            LXMaterialEntry.m_textureImageScale = std::atof(textureImageScaleAt->Value());
        }

        // only add if material ID is non-zero
        if (LXMaterialEntry.m_ID > 0)
        {
            outLandXMLMaterials.m_MaterialMap[LXMaterialEntry.m_ID] = LXMaterialEntry;
        }

        LXMaterial = LXMaterial->NextSiblingElement("Material");
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseSurface(XMLElement* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface)
{
    bool retStat = true;

    ParseSurfaceBoundaries(LXSurfaceNode, outLandXMLSurface);

    // prepare separate surface meshes for each texture boundary material
    for (LandXMLTextureBoundary& LXbp : outLandXMLSurface.m_textureBoundaries)
    {
        LXbp.m_name = inLandXMLMaterials.m_MaterialMap[LXbp.m_materialID].m_name;
    }

    // Create a single surface mesh for LandXML-1.0 files
    if (true == outLandXMLSurface.m_textureBoundaries.empty())
    {
        LandXMLTextureBoundary TBToAdd;

        TBToAdd.m_name = inLandXMLMaterials.m_MaterialMap.begin()->second.m_name;
        TBToAdd.m_materialID = inLandXMLMaterials.m_MaterialMap.begin()->second.m_ID;

        outLandXMLSurface.m_textureBoundaries.push_back(TBToAdd);
    }


    const XMLAttribute* surfaceName = LXSurfaceNode->FindAttribute("name");
    if (surfaceName)
    {
        outLandXMLSurface.m_name = surfaceName->Value();
    }

    XMLElement* LXSurfaceDef = LXSurfaceNode->FirstChildElement("Definition");

    if (LXSurfaceDef)
    {
        ParseSurfacePoints(LXSurfaceDef, outLandXMLSurface);
        ParseSurfaceFaces(LXSurfaceDef, outLandXMLSurface);
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseSurfacePoints(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface)
{
    bool retval = false;

    if (!LXSurfaceDefNode)
    {
        return retval;
    }

    XMLElement* LXSurfacePnts = LXSurfaceDefNode->FirstChildElement("Pnts");

    if (!LXSurfacePnts)
    {
        return retval;
    }

    XMLElement* LXSurfacePnt = LXSurfacePnts->FirstChildElement("P");

    // Parse the surface points
    while (LXSurfacePnt)
    {
        LandXMLPoint3D surfPnt;

        if (ParsePoint3D(LXSurfacePnt, surfPnt))
        {
            outLandXMLSurface.m_surfacePoints.push_back(surfPnt);

            // find min surf coords
            if (surfPnt.x < outLandXMLSurface.m_minSurfPoint.x)
            {
                outLandXMLSurface.m_minSurfPoint.x = surfPnt.x;
            }

            if (surfPnt.y < outLandXMLSurface.m_minSurfPoint.y)
            {
                outLandXMLSurface.m_minSurfPoint.y = surfPnt.y;
            }

            if (surfPnt.z < outLandXMLSurface.m_minSurfPoint.z)
            {
                outLandXMLSurface.m_minSurfPoint.z = surfPnt.z;
            }

            // find max surf coords
            if (surfPnt.x > outLandXMLSurface.m_maxSurfPoint.x)
            {
                outLandXMLSurface.m_maxSurfPoint.x = surfPnt.x;
            }

            if (surfPnt.y > outLandXMLSurface.m_maxSurfPoint.y)
            {
                outLandXMLSurface.m_maxSurfPoint.y = surfPnt.y;
            }

            if (surfPnt.z > outLandXMLSurface.m_maxSurfPoint.z)
            {
                outLandXMLSurface.m_maxSurfPoint.z = surfPnt.z;
            }
        }

        LXSurfacePnt = LXSurfacePnt->NextSiblingElement("P");
    }

    if (outLandXMLSurface.m_surfacePoints.size() > 2)
    {
        retval = true;
    }

    return retval;
}

bool LXParser::ParseSurfaceFaces(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface)
{
    bool retval = false;

    if (!LXSurfaceDefNode)
    {
        return retval;
    }

    XMLElement* LXSurfaceFaces = LXSurfaceDefNode->FirstChildElement("Faces");

    if (!LXSurfaceFaces)
    {
        return retval;
    }

    XMLElement* LXSurfaceFace = LXSurfaceFaces->FirstChildElement("F");
    while (LXSurfaceFace)
    {
        LandXMLSurfaceFace surfFace;

        if (ParseFace(LXSurfaceFace, surfFace))
        {
            // LandXML surface ids start at 1, reduce point IDs by 1 to make vertex face point ids match zero based surface TIN point array

            UINT facedPointId1 = surfFace.m_pointIndices[0] - 1;
            UINT facedPointId2 = surfFace.m_pointIndices[1] - 1;
            UINT facedPointId3 = surfFace.m_pointIndices[2] - 1;

            LandXMLPoint3D p1 = outLandXMLSurface.m_surfacePoints[facedPointId1];
            LandXMLPoint3D p2 = outLandXMLSurface.m_surfacePoints[facedPointId2];
            LandXMLPoint3D p3 = outLandXMLSurface.m_surfacePoints[facedPointId3];

            LandXMLPoint3D faceCenterPnt(((p1.x + p2.x + p3.x) / 3), ((p1.y + p2.y + p3.y) / 3), ((p1.z + p2.z + p3.z) / 3));

            if (outLandXMLSurface.m_textureBoundaries.size() > 1)
            {
                for (LandXMLTextureBoundary& LXbp : outLandXMLSurface.m_textureBoundaries)
                {
                    if (MathHelper::PointInPolygon(faceCenterPnt, LXbp.m_polylinePoints))
                    {
                        LXbp.m_surfaceFaces.push_back(surfFace);
                        break;
                    }
                }
            }
            else
            {
                if (outLandXMLSurface.m_textureBoundaries.size() > 0)
                {
                    outLandXMLSurface.m_textureBoundaries.front().m_surfaceFaces.push_back(surfFace);
                }
            }

            retval = true;
        }

        LXSurfaceFace = LXSurfaceFace->NextSiblingElement("F");
    }

    return retval;
}

bool LXParser::ParseSurfaceBoundaries(XMLElement* LXSurfaceNode, LandXMLSurface& outLandXMLSurface)
{
    bool retStat = false;

    XMLElement* LXSurfaceData = LXSurfaceNode->FirstChildElement("SourceData");
    if (!LXSurfaceData)
    {
        return retStat;
    }

    XMLElement* LXSurfaceBoundaries = LXSurfaceData->FirstChildElement("Boundaries");
    if (!LXSurfaceBoundaries)
    {
        return retStat;
    }

    XMLElement* LXSurfaceBndry = LXSurfaceBoundaries->FirstChildElement("Boundary");

    while (LXSurfaceBndry)
    {
        XMLNode* LXPntList = LXSurfaceBndry->FirstChildElement("PntList3D");

        if (LXPntList && LXPntList->FirstChild())
        {
            LandXMLTextureBoundary bndryPoly;

            const XMLAttribute* typeAt = LXSurfaceBndry->FindAttribute("bndType");
            if (typeAt)
            {
                bndryPoly.m_description = typeAt->Value();
            }

            const XMLAttribute* materialAt = LXSurfaceBndry->FindAttribute("m");
            if (materialAt)
            {
                bndryPoly.m_materialID = std::atoi(materialAt->Value());
            }

            ParsePointList3D(LXPntList, bndryPoly.m_polylinePoints);
            bndryPoly.m_area = MathHelper::PolygonArea(bndryPoly.m_polylinePoints);

            outLandXMLSurface.m_textureBoundaries.push_back(bndryPoly);
        }

        LXSurfaceBndry = LXSurfaceBndry->NextSiblingElement("Boundary");
    }

    std::function<bool(LandXMLTextureBoundary, LandXMLTextureBoundary)> sort
        = [](LandXMLTextureBoundary x, LandXMLTextureBoundary y)
        {
            return (x.m_area < y.m_area);
        };

    // sort texture boundary polygons by size
    outLandXMLSurface.m_textureBoundaries.sort(sort);

    retStat = true;
    return retStat;
}

// Parse a single point element
bool LXParser::ParsePoint3D(XMLNode* LXPointList, LandXMLPoint3D& outReturnPoint3D)
{
    std::vector<std::string> pointYXZArray;

    SplitCData(LXPointList, pointYXZArray);

    if (pointYXZArray.size() >= 3)
    {
        outReturnPoint3D.y = std::atof(pointYXZArray[0].c_str());
        outReturnPoint3D.x = std::atof(pointYXZArray[1].c_str());
        outReturnPoint3D.z = std::atof(pointYXZArray[2].c_str());
    }

    return (pointYXZArray.size() >= 3);
}

// Parse a point list element
bool LXParser::ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& outReturnPointList)
{
    std::vector<std::string> pointYXZArray;
    SplitCData(LXPointList, pointYXZArray);

    for (int j = 0; j < (pointYXZArray.size() - 3); j += 3)
    {
        LandXMLPoint3D pointToAdd;

        pointToAdd.y = std::atof(pointYXZArray[j].c_str());
        pointToAdd.x = std::atof(pointYXZArray[j + 1].c_str());
        pointToAdd.z = std::atof(pointYXZArray[j + 2].c_str());

        outReturnPointList.push_back(pointToAdd);
    }


    return (outReturnPointList.size() > 0);
}

bool LXParser::ParseFace(XMLNode* LXFaceList, LandXMLSurfaceFace& outReturnFace)
{
    std::vector<std::string> pointYXZArray;

    SplitCData(LXFaceList, pointYXZArray);

    if (pointYXZArray.size() >= 3)
    {
        outReturnFace.m_pointIndices.push_back(std::atoi(pointYXZArray[0].c_str()));
        outReturnFace.m_pointIndices.push_back(std::atoi(pointYXZArray[1].c_str()));
        outReturnFace.m_pointIndices.push_back(std::atoi(pointYXZArray[2].c_str()));
    }

    return (outReturnFace.m_pointIndices.size() >= 3);
}

void LXParser::SplitCData(XMLNode* LXPointList, std::vector<std::string>& outPointYXZArray)
{
    std::string pointList = LXPointList->FirstChild()->Value();

    std::regex reg("\\s+");
    std::sregex_token_iterator iter(pointList.begin(), pointList.end(), reg, -1);
    std::sregex_token_iterator end;

    std::vector<std::string> pointYXZArray(iter, end);

    outPointYXZArray = pointYXZArray;
}

void LXParser::LXColor2RGB(const std::string& colorValueStr, float& R, float& G, float& B)
{
    char tokBuffer[MAX_PATH] = { 0 };
    strcpy(tokBuffer, colorValueStr.c_str());

    char* nextNum = nullptr;
    float r = 0.0, g = 0.0, b = 0.0;

    if (strlen(tokBuffer) > 1)
    {
        nextNum = strtok(tokBuffer, ",");
        if (nextNum)
            r = atof(nextNum);

        nextNum = strtok(NULL, ",");
        if (nextNum)
            g = atof(nextNum);

        nextNum = strtok(NULL, ",");
        if (nextNum)
            b = atof(nextNum);
    }

    R = (r / 255.0);
    G = (g / 255.0);
    B = (b / 255.0);
}

}