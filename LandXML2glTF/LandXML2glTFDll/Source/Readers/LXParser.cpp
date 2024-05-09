#include "Readers/LXParser.h"
#include "Helpers/MathHelper.h"
#include <functional>
#include <iostream>
#include <fstream>
#include <filesystem>


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

    if (!LXRoot || (strcmp(LXRoot->Name(), "LandXML") != 0))
    {
        std::cout << "error: The XML file does not contain valid LandXML data: " << outLandXMLMDoc.m_fileName << "\n";
        return retStat;
    }

    // Parse Units and coordinate system
    XMLElement* LXUnits = LXRoot->FirstChildElement("Units");

    // The Units element and linearUnit are REQUIRED for any meaningful conversion
    if (!LXUnits)
    {
        std::cout << "error: The LandXML file is missing the required <Units> element.\n";
        return retStat;
    }

    XMLElement* LXUnit = LXUnits->FirstChildElement();

    if (LXUnit)
    {
        const XMLAttribute* linearUnitAT = LXUnit->FindAttribute("linearUnit");

        if (!linearUnitAT)
        {
            std::cout << "error: The LandXML file is missing the required <Units> element attribute \"linearUnit\".\n";
            return retStat;
        }

        outLandXMLMDoc.m_units.m_linearUnitString = linearUnitAT->Value();

        if (outLandXMLMDoc.m_units.m_linearUnitString.empty())
        {
            std::cout << "error: The LandXML file is missing a value for the required <Units> element attribute \"linearUnit\".\n";
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

bool LXParser::ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLModel& outLandXMLMDoc, std::string& exePath)
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

    if (!ParseMaterialTable(LXMaterials, outLandXMLMDoc.m_landXMLMaterials, exePath))
    {
        std::cout << "error: Unable to find LandXML <MaterialTable> element in ./data/DefaultTexture.xml\n";
        return retStat;
    }

    XMLElement* LXCgPoints = LXRoot->FirstChildElement("CgPoints");
    while (LXCgPoints)
    {
        ParseCgPoints(LXCgPoints, outLandXMLMDoc);

        LXCgPoints = LXCgPoints->NextSiblingElement("CgPoints");
    }

    // Parse Surfaces
    XMLElement* LXSurfaces = LXRoot->FirstChildElement("Surfaces");
    if (LXSurfaces)
    {
        XMLElement* LXSurface = LXSurfaces->FirstChildElement("Surface");

        while (LXSurface)
        {
            LandXMLSurface* LXSurfaceModel = new LandXMLSurface();

            ParseSurface(LXSurface, outLandXMLMDoc.m_landXMLMaterials, *LXSurfaceModel, outLandXMLMDoc.m_landxmlPoints);

            outLandXMLMDoc.m_landxmlSurfaces.push_back(LXSurfaceModel);

            LXSurface = LXSurface->NextSiblingElement("Surface");
        }
    }


    // Parse polyline data types
    XMLElement* LXAlignments = LXRoot->FirstChildElement("Alignments");
    while (LXAlignments)
    {
        ParseAlignments(LXAlignments, outLandXMLMDoc);

        LXAlignments = LXAlignments->NextSiblingElement("Alignments");
    }


    XMLElement* LXParcels = LXRoot->FirstChildElement("Parcels");
    while (LXParcels)
    {
        ParseParcels(LXParcels, outLandXMLMDoc);

        LXParcels = LXParcels->NextSiblingElement("Parcels");
    }


    XMLElement* LXPlanFeatures = LXRoot->FirstChildElement("PlanFeatures");
    while (LXPlanFeatures)
    {
        ParsePlanFeatures(LXPlanFeatures, outLandXMLMDoc);

        LXPlanFeatures = LXPlanFeatures->NextSiblingElement("PlanFeatures");
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseMaterialTable(XMLElement* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials, std::string& exeDataPath)
{
    bool retStat = false;
    tinyxml2::XMLDocument defaultMatDoc;
    std::filesystem::path defaultMatDocPath = exeDataPath + "/DefaultTexture.xml";
    std::filesystem::path defaultMatDocPathABS = std::filesystem::absolute(defaultMatDocPath);

    std::string defaultMatDocFullPath = defaultMatDocPathABS.string().c_str();

    XMLElement* defaultMatRootElem = nullptr;
    XMLElement* LXMaterialTableToUse = nullptr;

    // probably a LandXML 1.0 or 1.2 file, so load the default texture material from data file
    if (!LXMaterialsNode)
    {
        if (defaultMatDoc.LoadFile((defaultMatDocFullPath.c_str())) == XML_SUCCESS)
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

                        if (TextureHexString && TextureHexString->FirstChild())
                        {
                            //size_t hexStrLen = 0;
                            //hexStrLen = strlen(TextureHexString->FirstChild()->Value()) + 1;
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

bool LXParser::ParseSurface(XMLElement* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints)
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
        ParseSurfacePoints(LXSurfaceDef, outLandXMLSurface, landxmlCGPoints);
        ParseSurfaceFaces(LXSurfaceDef, outLandXMLSurface);
    }

    retStat = true;
    return retStat;
}

bool LXParser::ParseSurfacePoints(XMLElement* LXSurfaceDefNode, LandXMLSurface& outLandXMLSurface, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints)
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

    outLandXMLSurface.m_surfacePointCount = 0;
    // Parse the surface points
    while (LXSurfacePnt)
    {
        LandXMLPoint3D surfPnt;

        unsigned int pointID = 1, pointIDFix = 0;

        const XMLAttribute* pIDAtt = LXSurfacePnt->FindAttribute("id");
        if (pIDAtt)
        {
            pointID = std::atoi(pIDAtt->Value());

            if (outLandXMLSurface.m_surfacePointCount > 0)
            {
                pointIDFix = pointID - 1;
            }
        }

        if (ParsePoint(LXSurfacePnt, surfPnt, landxmlCGPoints))
        {
            outLandXMLSurface.m_surfacePointCount++;
            outLandXMLSurface.m_surfacePoints[pointIDFix] = surfPnt;

            // find min surf coords

            if (surfPnt < outLandXMLSurface.m_minSurfPoint)
            {
                outLandXMLSurface.m_minSurfPoint = surfPnt;
            }

            //if (surfPnt.x < outLandXMLSurface.m_minSurfPoint.x)
            //{
            //    outLandXMLSurface.m_minSurfPoint.x = surfPnt.x;
            //}

            //if (surfPnt.y < outLandXMLSurface.m_minSurfPoint.y)
            //{
            //    outLandXMLSurface.m_minSurfPoint.y = surfPnt.y;
            //}

            //if (surfPnt.z < outLandXMLSurface.m_minSurfPoint.z)
            //{
            //    outLandXMLSurface.m_minSurfPoint.z = surfPnt.z;
            //}

            // find max surf coords
            if (surfPnt > outLandXMLSurface.m_maxSurfPoint)
            {
                outLandXMLSurface.m_maxSurfPoint = surfPnt;
            }

            //if (surfPnt.x > outLandXMLSurface.m_maxSurfPoint.x)
            //{
            //    outLandXMLSurface.m_maxSurfPoint.x = surfPnt.x;
            //}

            //if (surfPnt.y > outLandXMLSurface.m_maxSurfPoint.y)
            //{
            //    outLandXMLSurface.m_maxSurfPoint.y = surfPnt.y;
            //}

            //if (surfPnt.z > outLandXMLSurface.m_maxSurfPoint.z)
            //{
            //    outLandXMLSurface.m_maxSurfPoint.z = surfPnt.z;
            //}
        }

        LXSurfacePnt = LXSurfacePnt->NextSiblingElement("P");
    }

    if (outLandXMLSurface.m_surfacePointCount > 2)
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

            unsigned int facedPointId1 = surfFace.m_pointIndices[0] - 1U;
            unsigned int facedPointId2 = surfFace.m_pointIndices[1] - 1U;
            unsigned int facedPointId3 = surfFace.m_pointIndices[2] - 1U;

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

            if (!strcmp(bndryPoly.m_description.c_str(), "texture"))
            {

                const XMLAttribute* materialAt = LXSurfaceBndry->FindAttribute("m");
                if (materialAt)
                {
                    bndryPoly.m_materialID = std::atoi(materialAt->Value());
                }

                ParsePointList3D(LXPntList, bndryPoly.m_polylinePoints);
                bndryPoly.m_area = MathHelper::GetPolygonArea(bndryPoly.m_polylinePoints);

                outLandXMLSurface.m_textureBoundaries.push_back(bndryPoly);
            }
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


bool LXParser::ParseAlignments(XMLElement* LXAlignments, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    if (!LXAlignments)
    {
        return retStat;
    }

    XMLElement* LXAlignment = LXAlignments->FirstChildElement("Alignment");

    while (LXAlignment)
    {
        LandXMLAlignment toAdd;

        if (ParsePolyline(LXAlignment, &toAdd, outLandXMLMDoc.m_landxmlPoints))
        {
            outLandXMLMDoc.m_landxmlAlignments.push_back(toAdd);
            retStat = true;
        }

        LXAlignment = LXAlignment->NextSiblingElement("Alignment");
    }

    return retStat;
}

bool LXParser::ParseParcels(XMLElement* LXParcels, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    XMLElement* LXParcel = LXParcels->FirstChildElement("Parcel");

    while (LXParcel)
    {
        LandXMLParcel toAdd;

        if (ParsePolyline(LXParcel, &toAdd, outLandXMLMDoc.m_landxmlPoints))
        {
            outLandXMLMDoc.m_landxmlParcels.push_back(toAdd);
            retStat = true;
        }

        LXParcel = LXParcel->NextSiblingElement("Parcel");
    }

    return retStat;
}

bool LXParser::ParsePlanFeatures(XMLElement* LXPlanFeatures, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    XMLElement* LXPlanFeature = LXPlanFeatures->FirstChildElement("PlanFeature");

    while (LXPlanFeature)
    {
        LandXMLPolyline toAdd;

        if (ParsePolyline(LXPlanFeature, &toAdd, outLandXMLMDoc.m_landxmlPoints))
        {
            outLandXMLMDoc.m_landxmlPlanFeatures.push_back(toAdd);
            retStat = true;
        }

        LXPlanFeature = LXPlanFeature->NextSiblingElement("PlanFeature");
    }

    return retStat;
}

bool LXParser::ParseCgPoints(XMLElement* LXCgPoints, LandXMLModel& outLandXMLMDoc)
{
    bool retStat = false;

    XMLElement* LXCgPoint = LXCgPoints->FirstChildElement("CgPoint");

    while (LXCgPoint)
    {
        LandXMLPoint3D toAdd;

        const XMLAttribute* nameAt = LXCgPoint->FindAttribute("name");
        if (nameAt)
        {
            toAdd.m_name = nameAt->Value();
        }

        if (ParsePoint(LXCgPoint, toAdd, outLandXMLMDoc.m_landxmlPoints))
        {
            const XMLAttribute* descAt = LXCgPoint->FindAttribute("desc");
            if (descAt)
            {
                toAdd.m_description = descAt->Value();
            }

            const XMLAttribute* codeAt = LXCgPoint->FindAttribute("code");
            if (codeAt)
            {
                toAdd.m_code = codeAt->Value();
            }

            const XMLAttribute* materialAt = LXCgPoint->FindAttribute("m");
            if (materialAt)
            {
                toAdd.m_materialID = std::atoi(materialAt->Value());
            }

            outLandXMLMDoc.m_landxmlPoints[toAdd.m_name] = toAdd;
            retStat = true;
        }

        LXCgPoint = LXCgPoint->NextSiblingElement("CgPoint");
    }

    return retStat;
}

bool LXParser::ParsePolyline(XMLElement* LXPolyline, LandXMLPolyline* LXPoly, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints)
{
    bool retStat = false;

    if (!LXPolyline || !LXPoly)
    {
        return retStat;
    }

    const XMLAttribute* nameAt = LXPolyline->FindAttribute("name");
    if (nameAt)
    {
        LXPoly->m_name = nameAt->Value();
    }

    const XMLAttribute* descAt = LXPolyline->FindAttribute("desc");
    if (descAt)
    {
        LXPoly->m_description = descAt->Value();
    }

    const XMLAttribute* materialAt = LXPolyline->FindAttribute("m");
    if (materialAt)
    {
        LXPoly->m_materialID = std::atoi(materialAt->Value());
    }
    else
    {
        LXPoly->m_materialID = 1;
    }

    XMLElement* LXCoordGeom = LXPolyline->FirstChildElement("CoordGeom");

    if (LXCoordGeom)
    {
        retStat = ParseCoordGeom(LXCoordGeom, LXPoly->m_polylinePoints, landxmlCGPoints);
    }

    return retStat;
}

bool LXParser::ParseCoordGeom(XMLElement* LXCoordGeom, std::vector<LandXMLPoint3D>& OutReturnPointList, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints)
{
    bool retStat = false;

    if (LXCoordGeom)
    {
        XMLElement* LXSegment = LXCoordGeom->FirstChildElement();

        while (LXSegment)
        {
            LandXMLPoint3D startPnt, endPnt, centerPoint;

            XMLElement* LXStart = LXSegment->FirstChildElement("Start");
            XMLElement* LXEnd = LXSegment->FirstChildElement("End");

            if (!LXStart || !LXEnd)
            {
                LXSegment = LXSegment->NextSiblingElement();
                continue;
            }

            if (!strcmp(LXSegment->Name(), "Line"))
            {
                if (ParsePoint(LXStart, startPnt, landxmlCGPoints))
                {
                    OutReturnPointList.push_back(startPnt);
                }

                if (ParsePoint(LXEnd, endPnt, landxmlCGPoints))
                {
                    OutReturnPointList.push_back(endPnt);
                }
            }
            else if (!strcmp(LXSegment->Name(), "Curve"))
            {
                if (ParsePoint(LXStart, startPnt, landxmlCGPoints))
                {
                    OutReturnPointList.push_back(startPnt);
                }

                ParsePoint(LXEnd, endPnt, landxmlCGPoints);

                //const XMLAttribute* rotAtt = LXSegment->FindAttribute("rot");

                XMLElement* LXCenter = LXSegment->FirstChildElement("Center");

                if (ParsePoint(LXCenter, centerPoint, landxmlCGPoints))
                {
                    std::vector<LandXMLPoint3D> curveTessPointList;

                    if (MathHelper::Tesselate2DCurve(startPnt, centerPoint, endPnt, curveTessPointList))
                    {
                        OutReturnPointList.reserve(OutReturnPointList.size() + curveTessPointList.size());
                        OutReturnPointList.insert(OutReturnPointList.end(), curveTessPointList.begin(), curveTessPointList.end());
                    }
                }

                OutReturnPointList.push_back(endPnt);
            }

            LXSegment = LXSegment->NextSiblingElement();

            retStat = true;
        }
    }

    return retStat;
}


// Parse a single point element
bool LXParser::ParsePoint(XMLElement* LXPointList, LandXMLPoint3D& outReturnPoint3D, std::map<std::string, LandXMLPoint3D>& landxmlCGPoints)
{
    if (!LXPointList)
    {
        return false;
    }

    std::vector<std::string> pointYXZArray;
    outReturnPoint3D.z = 0.0;

    SplitCData(LXPointList, pointYXZArray);

    if (pointYXZArray.size() >= 2)
    {
        outReturnPoint3D.y = std::atof(pointYXZArray[0].c_str());
        outReturnPoint3D.x = std::atof(pointYXZArray[1].c_str());
    }

    if (pointYXZArray.size() >= 3)
    {
        outReturnPoint3D.z = std::atof(pointYXZArray[2].c_str());
    }

    if (pointYXZArray.size() < 2)
    {
        const XMLAttribute* pntRefAt = LXPointList->FindAttribute("pntRef");
        if (pntRefAt && pntRefAt->Value())
        {
            std::string pointRefName = pntRefAt->Value();

            outReturnPoint3D = landxmlCGPoints[pointRefName];
        }
    }

    return ((outReturnPoint3D.x != 0.0) && (outReturnPoint3D.y != 0.0));
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
    if (LXPointList->FirstChild())
    {
        std::string pointList = LXPointList->FirstChild()->Value();

        std::regex reg("\\s+");
        std::sregex_token_iterator iter(pointList.begin(), pointList.end(), reg, -1);
        std::sregex_token_iterator end;

        std::vector<std::string> pointYXZArray(iter, end);

        outPointYXZArray = pointYXZArray;
    }
}

void LXParser::LXColor2RGB(const std::string& colorValueStr, float& R, float& G, float& B)
{
    char tokBuffer[255] = { 0 };
    strcpy(tokBuffer, colorValueStr.c_str());

    char* nextNum = nullptr;
    float r = 0.0, g = 0.0, b = 0.0;

    if (strlen(tokBuffer) > 1)
    {
        nextNum = strtok(tokBuffer, ",");
        if (nextNum)
        {
            r = (float)atof(nextNum);
        }

        nextNum = strtok(nullptr, ",");
        if (nextNum)
        {
            g = (float)atof(nextNum);
        }

        nextNum = strtok(nullptr, ",");
        if (nextNum)
        {
            b = (float)atof(nextNum);
        }
    }

    R = (r / 255);
    G = (g / 255);
    B = (b / 255);
}

}