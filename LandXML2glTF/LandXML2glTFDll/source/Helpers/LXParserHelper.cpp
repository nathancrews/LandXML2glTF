#include "Helpers/LXParserHelper.h"
#include <functional>

namespace LANDXML2GLTF
{

    void LXParserHelper::ParseLandXMLFile(tinyxml2::XMLDocument* LXDocument, LandXMLMaterialTable& outLandXMLMaterials, std::vector<LandXMLSurface*>& outLandxmlSurfaces)
    {
        if (!LXDocument || !LXDocument->RootElement())
        {
            return;
        }

        XMLElement* LXRoot = LXDocument->RootElement();

        // Parse Material table

        XMLElement* LXMaterials = LXRoot->FirstChildElement("MaterialTable");

        // Not all LandXML files contain materials, so create a single texture material table to use
        if (LXMaterials)
        {
            ParseMaterialTable(LXMaterials, outLandXMLMaterials);
        }

        // Parse Surfaces
        XMLElement* LXSurfaces = LXRoot->FirstChildElement("Surfaces");
        if (!LXSurfaces)
        {
            return;
        }

        XMLElement* LXSurface = LXSurfaces->FirstChildElement("Surface");

        while (LXSurface)
        {
            LandXMLSurface* LXSurfaceModel = new LandXMLSurface();

            ParseSurface(LXSurface, outLandXMLMaterials, *LXSurfaceModel);

            outLandxmlSurfaces.push_back(LXSurfaceModel);

            LXSurface = LXSurface->NextSiblingElement("Surface");
        }

    }

    void LXParserHelper::ParseMaterialTable(XMLNode* LXMaterialsNode, LandXMLMaterialTable& outLandXMLMaterials)
    {
        if (!LXMaterialsNode)
        {
            return;
        }

        XMLElement* LXMaterial = LXMaterialsNode->FirstChildElement("Material");
        XMLElement* LXTextureImageTable = LXMaterialsNode->NextSiblingElement("TextureImageTable");


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

    }

    void LXParserHelper::ParseSurface(XMLNode* LXSurfaceNode, LandXMLMaterialTable& inLandXMLMaterials, LandXMLSurface& outLandXMLSurface)
    {
        ParseSurfaceBoundaries(LXSurfaceNode, outLandXMLSurface);

        // prepare a seperate surface mesh for each texture boundary material

        for (LandXMLPolyline LXbp : outLandXMLSurface.m_textureBoundaries)
        {
            std::unordered_map<int, LandXMLMaterial>::const_iterator findMatID = inLandXMLMaterials.m_MaterialMap.find(LXbp.m_materialID);

            if (findMatID != inLandXMLMaterials.m_MaterialMap.end())
            {
                std::unordered_map<int, LandXMLSurfaceMesh*>::const_iterator findIt = outLandXMLSurface.m_surfaceMeshes.find(LXbp.m_materialID);

                // not found
                if (findIt == outLandXMLSurface.m_surfaceMeshes.end())
                {
                    outLandXMLSurface.m_surfaceMeshes[LXbp.m_materialID] = new LandXMLSurfaceMesh();
                    outLandXMLSurface.m_surfaceMeshes[LXbp.m_materialID]->m_materialID = LXbp.m_materialID;
                    outLandXMLSurface.m_surfaceMeshes[LXbp.m_materialID]->m_materialName = inLandXMLMaterials.m_MaterialMap[LXbp.m_materialID].m_name;
                }
            }
        }

        if (true == outLandXMLSurface.m_surfaceMeshes.empty())
        {
            return;
        }

        LandXMLSurfaceMesh* addToMesh = outLandXMLSurface.m_surfaceMeshes.begin()->second;


        // LandXML surface ids start at 1, so add a dummy point so vertex faces ids match point array
        LandXMLPoint3D dummyZeroSurfPnt;
        outLandXMLSurface.m_surfacePoints.push_back(dummyZeroSurfPnt);

        XMLElement* LXSurfaceDef = LXSurfaceNode->FirstChildElement("Definition");

        if (LXSurfaceDef)
        {
            XMLElement* LXSurfacePnts = LXSurfaceDef->FirstChildElement("Pnts");
            XMLElement* LXSurfaceFaces = LXSurfaceDef->FirstChildElement("Faces");
            XMLElement* LXSurfacePnt = LXSurfacePnts->FirstChildElement("P");

            // Parse the surface points
            while (LXSurfacePnt)
            {
                LandXMLPoint3D surfPnt;

                if (ParsePoint3D(LXSurfacePnt, surfPnt))
                {
                    outLandXMLSurface.m_surfacePoints.push_back(surfPnt);
                }

                LXSurfacePnt = LXSurfacePnt->NextSiblingElement("P");
            }

            XMLElement* LXSurfaceFace = LXSurfaceFaces->FirstChildElement("F");
            while (LXSurfaceFace)
            {
                LandXMLPoint3D surfPnt;

                if (ParsePoint3D(LXSurfaceFace, surfPnt))
                {
                    LandXMLSurfaceFace surfFace;

                    surfFace.m_pointIndices.push_back((int)surfPnt.y);
                    surfFace.m_pointIndices.push_back((int)surfPnt.x);
                    surfFace.m_pointIndices.push_back((int)surfPnt.z);

                    LandXMLPoint3D p1 = outLandXMLSurface.m_surfacePoints[surfFace.m_pointIndices[0]];
                    LandXMLPoint3D p2 = outLandXMLSurface.m_surfacePoints[surfFace.m_pointIndices[1]];
                    LandXMLPoint3D p3 = outLandXMLSurface.m_surfacePoints[surfFace.m_pointIndices[2]];

                    LandXMLPoint3D faceCenterPnt(((p1.x + p2.x + p3.x) / 3), ((p1.y + p2.y + p3.y) / 3), ((p1.z + p2.z + p3.z) / 3));

                    for (LandXMLPolyline LXbp : outLandXMLSurface.m_textureBoundaries)
                    {
                        if (PointInPolygon(faceCenterPnt, LXbp.m_polylinePoints))
                        {
                            addToMesh = outLandXMLSurface.m_surfaceMeshes[LXbp.m_materialID];
                            break;
                        }
                    }

                    if (addToMesh)
                    {
                        addToMesh->m_surfaceFaces.push_back(surfFace);
                    }
                }

                LXSurfaceFace = LXSurfaceFace->NextSiblingElement("F");
            }

        }

    }

    void LXParserHelper::ParseSurfaceBoundaries(XMLNode* LXSurfaceNode, LandXMLSurface& outLandXMLSurface)
    {
        XMLElement* LXSurfaceData = LXSurfaceNode->FirstChildElement("SourceData");
        if (!LXSurfaceData)
        {
            return;
        }

        XMLElement* LXSurfaceBoundaries = LXSurfaceData->FirstChildElement("Boundaries");
        if (!LXSurfaceBoundaries)
        {
            return;
        }

        XMLElement* LXSurfaceBndry = LXSurfaceBoundaries->FirstChildElement("Boundary");

        while (LXSurfaceBndry)
        {
            XMLNode* LXPntList = LXSurfaceBndry->FirstChildElement("PntList3D");

            if (LXPntList && LXPntList->FirstChild())
            {
                LandXMLPolyline bndryPoly;

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
                bndryPoly.m_area = PolygonArea(bndryPoly.m_polylinePoints);

                outLandXMLSurface.m_textureBoundaries.push_back(bndryPoly);
            }

            LXSurfaceBndry = LXSurfaceBndry->NextSiblingElement("Boundary");
        }

        std::function<bool(LandXMLPolyline, LandXMLPolyline)> sort
            = [](LandXMLPolyline x, LandXMLPolyline y)
            {
                return (x.m_area < y.m_area);
            };

        // sort polylines by size
        outLandXMLSurface.m_textureBoundaries.sort(sort);

    }

    // Parse a single point element
    bool LXParserHelper::ParsePoint3D(XMLNode* LXPointList, LandXMLPoint3D& outReturnPoint3D)
    {
        std::vector<std::string> pointYXZArray;

        SplitPointCData(LXPointList, pointYXZArray);

        if (pointYXZArray.size() >= 3)
        {
            outReturnPoint3D.y = std::atof(pointYXZArray[0].c_str());
            outReturnPoint3D.x = std::atof(pointYXZArray[1].c_str());
            outReturnPoint3D.z = std::atof(pointYXZArray[2].c_str());
        }

        return (pointYXZArray.size() >= 3);
    }

    // Parse a point list element
    bool LXParserHelper::ParsePointList3D(XMLNode* LXPointList, std::vector<LandXMLPoint3D>& outReturnPointList)
    {
        std::vector<std::string> pointYXZArray;
        SplitPointCData(LXPointList, pointYXZArray);

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

    void LXParserHelper::SplitPointCData(XMLNode* LXPointList, std::vector<std::string>& outPointYXZArray)
    {
        std::string pointList = LXPointList->FirstChild()->Value();

        std::regex reg("\\s+");
        std::sregex_token_iterator iter(pointList.begin(), pointList.end(), reg, -1);
        std::sregex_token_iterator end;

        std::vector<std::string> pointYXZArray(iter, end);

        outPointYXZArray = pointYXZArray;
    }

    bool LXParserHelper::PointInPolygon(LandXMLPoint3D& point, std::vector<LandXMLPoint3D>& polygonPoints)
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

    double LXParserHelper::PolygonArea(std::vector<LandXMLPoint3D>& polygonPoints)
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
}