#include "Tools/PointcloudIO.h"

#pragma warning(push)
#pragma warning(disable: 4201 4324 4263 4264 4265 4266 4267 4512 4996 4702 4244 4250 4068)

#include "open3d/io/FileFormatIO.h"
#include "open3d/io/PointCloudIO.h"
#include "open3d/geometry/KDTreeFlann.h"
#include "open3d/geometry/KDTreeSearchParam.h"

#include <opencv2/opencv.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/io/LasReader.hpp>
#include <pdal/io/PtsReader.hpp>
#include <pdal/io/PlyReader.hpp>
#include <pdal/io/PcdReader.hpp>
#include <pdal/io/TextReader.hpp>
#include <pdal/io/LasHeader.hpp>

using namespace pdal;
using namespace pdal::las;

#include "pcl/PCLHeader.h"
#include <pcl/io/pcd_io.h>
#include <pcl/octree/octree_pointcloud.h>
#include <pcl/octree/octree_search.h>

namespace NCraftPointCloudTools
{

UINT LoadPointCloudFile(NCraftPointCloudTools::FileProcessPackage& outLoadResults, pcl::PointXYZRGB* mergedBasePoint)
{
    int pointCount = 0;
    double execExecTotal = 0.0;
    double exeTime = 0.0;

    if (outLoadResults.m_fileSize < 10)
    {
        return pointCount;
    }

    utility::Timer timer2;
    timer2.Start();

    if (!outLoadResults.m_FileName.extension().compare(".las") ||
        !outLoadResults.m_FileName.extension().compare(".laz") ||
        !outLoadResults.m_FileName.extension().compare(".LAS") ||
        !outLoadResults.m_FileName.extension().compare(".LAZ"))
    {
        std::shared_ptr<geometry::PointCloud> new_cloud_ptr = std::make_shared<geometry::PointCloud>();
        if (new_cloud_ptr)
        {

            pointCount = LoadLASorLAZToO3DCloud(outLoadResults.m_FileName, *new_cloud_ptr, mergedBasePoint);
            outLoadResults.m_pointCount = pointCount;
            new_cloud_ptr->SetName(outLoadResults.m_FileName.string());
            outLoadResults.m_cloudPtr = new_cloud_ptr;
        }
    }
    else if (!outLoadResults.m_FileName.extension().compare(".ply"))
    {
        std::shared_ptr<geometry::PointCloud> new_cloud_ptr = std::make_shared<geometry::PointCloud>();

        pointCount = LoadPLYToO3DCloud(outLoadResults.m_FileName, *new_cloud_ptr);
        outLoadResults.m_pointCount = pointCount;

        if (pointCount > 0)
        {
            new_cloud_ptr->SetName(outLoadResults.m_FileName.string());
            outLoadResults.m_cloudPtr = new_cloud_ptr;
        }

    }
    else if (!outLoadResults.m_FileName.extension().compare(".pts"))
    {
        std::shared_ptr<geometry::PointCloud> new_cloud_ptr = std::make_shared<geometry::PointCloud>();

        pointCount = LoadPTSToO3DCloud(outLoadResults.m_FileName, *new_cloud_ptr);
        outLoadResults.m_pointCount = pointCount;

        if (pointCount > 0)
        {
            new_cloud_ptr->SetName(outLoadResults.m_FileName.string());
            outLoadResults.m_cloudPtr = new_cloud_ptr;
        }

    }
    else if (!outLoadResults.m_FileName.extension().compare(".xyz"))
    {
        std::shared_ptr<geometry::PointCloud> new_cloud_ptr = std::make_shared<geometry::PointCloud>();

        pointCount = LoadXYZToO3DCloud(outLoadResults.m_FileName, *new_cloud_ptr);
        outLoadResults.m_pointCount = pointCount;

        if (pointCount > 0)
        {
            new_cloud_ptr->SetName(outLoadResults.m_FileName.string());
            outLoadResults.m_cloudPtr = new_cloud_ptr;
        }

    }
    else if (!outLoadResults.m_FileName.extension().compare(".pcd"))
    {
        std::shared_ptr<geometry::PointCloud> new_cloud_ptr = std::make_shared<geometry::PointCloud>();

        if (new_cloud_ptr)
        {
            if (io::ReadPointCloud(outLoadResults.m_FileName.string(), *new_cloud_ptr))
            {
                pointCount = new_cloud_ptr->points_.size();
                outLoadResults.m_pointCount = pointCount;

                if (pointCount > 0)
                {
                    new_cloud_ptr->SetName(outLoadResults.m_FileName.string());
                    outLoadResults.m_cloudPtr = new_cloud_ptr;
                }
            }
        }
    }

    timer2.Stop();
    double exeTimeInner = timer2.GetDurationInSecond();

    if (pointCount > 0)
    {
        outLoadResults.m_processTimeSeconds = exeTimeInner;

        utility::LogInfo("=====>Point cloud {}, points:{}, Load Duration: {} seconds\n",
                         outLoadResults.m_FileName.filename().string(), pointCount, exeTimeInner);
    }

    return pointCount;
}

UINT PackToSingleCloud(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& filesToLoad,
                       std::shared_ptr<geometry::PointCloud>& cloud_ptr, bool RemoveOutliers)
{
    UINT retval = 0;
    size_t neighbors = 20;
    double distRatio = 3.0;

    for (NCraftPointCloudTools::FileProcessPackage& fileToLoad : filesToLoad)
    {
        if (RemoveOutliers)
        {
            NCraftPointCloudTools::RemoveOutliers(neighbors, distRatio, fileToLoad.m_cloudPtr);
        }

        fileToLoad.m_cloudPtr->EstimateNormals(open3d::geometry::KDTreeSearchParamKNN(), true);
        fileToLoad.m_cloudPtr->NormalizeNormals();

        //assert(fileToLoad.m_cloudPtr->normals_.size() == fileToLoad.m_cloudPtr->colors_.size());

        //for (size_t p=0; p < fileToLoad.m_cloudPtr->normals_.size(); ++p)
        //{
        //    Eigen::Vector3d norm  = fileToLoad.m_cloudPtr->normals_[p];

        //    if (norm.x() < 0.05 && norm.y() < 0.05 && norm.z() > 0.95)
        //    {
        //        float newColor = fileToLoad.m_cloudPtr->colors_[p][1] + 0.5;
        //        newColor = std::clamp(newColor, 0.0f, 1.0f);
        //        fileToLoad.m_cloudPtr->colors_[p][1] = newColor;

        //    }

        //}


        if (fileToLoad.m_cloudPtr != nullptr && fileToLoad.m_cloudPtr->IsEmpty() == false)
        {
            *cloud_ptr += *fileToLoad.m_cloudPtr;

            retval++;
        }
    }
    return retval;
}

UINT LoadPointCloudFilesParallel(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& outLoadResults, 
                                 pcl::PointXYZRGB* mergedBasePoint)
{
    int pointCountNP = 0;
    int pointCountTotal = 0;
    double execExecTotal = 0.0;
    double exeTime = 0.0;

    utility::Timer timer;


    if (outLoadResults.size() == 0)
    {
        return 0;
    }

    timer.Start();

    if (outLoadResults.size() > 0)
    {
        if (outLoadResults[0].m_imageFileCacheOk == false)
        {
            pointCountNP = LoadPointCloudFile(outLoadResults[0], mergedBasePoint);

            pointCountTotal = pointCountNP;
        }

#pragma omp parallel for reduction (+ : pointCountTotal)
        for (int sz = 1; sz < outLoadResults.size(); ++sz)
        {
            int pointCount = 0;

            if (outLoadResults[sz].m_imageFileCacheOk == false)
            {
                utility::Timer timer2;
                timer2.Start();

                pointCount = LoadPointCloudFile(outLoadResults[sz], mergedBasePoint);

                timer2.Stop();
                double exeTimeInner = timer2.GetDurationInSecond();

                if (pointCount > 0)
                {
                    outLoadResults[sz].m_pointCount = pointCount;
                    outLoadResults[sz].m_processTimeSeconds = exeTimeInner;

                    utility::LogInfo("@@===>Threaded Point cloud {}, points:{}, Load Duration: {} seconds\n",
                                     outLoadResults[sz].m_FileName.filename().string(), pointCount, exeTimeInner);

                    pointCountTotal += pointCount;
                }
            }
        }
    }

    return pointCountTotal;
}


UINT LoadLASorLAZToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud, pcl::PointXYZRGB* pCommonBasePoint)
{
    double px = 0.0, py = 0.0, pz = 0.0;
    double pBasex = 0.0, pBasey = 0.0, pBasez = 0.0;
    int pointCount = 0;
    // color from point intensity factor
    double cfactor = 255.0 / 65536.0;
    uint64_t pointsInFile = 0;
    pcl::PointXYZRGB pclPoint, point;
    UINT pointStep = 1;
    UINT pointToPixel = 750000;
    PointTable table;
    Eigen::Vector3d o3dPoint, o3dColor;
    Options ops1;
    LasReader reader;

    ops1.add("filename", fileName.string());
    reader.setOptions(ops1);

    const LasHeader lsHeader = reader.header();

    reader.prepare(table);
    PointViewSet point_views = reader.execute(table);

    pointsInFile = lsHeader.pointCount();

    utility::LogInfo("LAS/LAZ::::Point cloud file: {}, points:{}", fileName.filename().string(), pointsInFile);

    if (lsHeader.pointCount() < 5)
    {
        return 0;
    }

    if (lsHeader.pointCount() > pointToPixel)
    {
        pointStep = lsHeader.pointCount() / pointToPixel;
    }

    if (pointStep == 0)
    {
        pointStep = 1;
    }

    pdal::BOX3D bnds = lsHeader.getBounds();
    double deltaElev = bnds.maxz - bnds.minz;
    double greyScaleFactor = 0.90;

    if (deltaElev < 0.001 || deltaElev > 200.0)
    {
        deltaElev = 200.0;

        bnds.maxz = bnds.minz + deltaElev;
    }

    utility::LogInfo("LAS...Point cloud min z = {}, max z = {}, views = {}, \n", bnds.minz, bnds.maxz, point_views.size());

    for (PointViewPtr point_view : point_views)
    {
        for (pdal::PointId idx = 0; idx < (point_view->size() - pointStep); idx += pointStep)
        {
            if (pCommonBasePoint && (idx == 0))
            {
                if (pCommonBasePoint->x == 0.0 && pCommonBasePoint->y == 0.0)
                {
                    pBasex = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                    pBasey = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                    pBasez = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

                    pCommonBasePoint->x = pBasex;
                    pCommonBasePoint->y = pBasey;
                    pCommonBasePoint->z = pBasez;
                    //utility::LogInfo("LAS/LAZ::::Setting common base point {}, {}, {}", pBasex, pBasey, pBasez);
                }
                else
                {
                    pBasex = pCommonBasePoint->x;
                    pBasey = pCommonBasePoint->y;
                    pBasez = pCommonBasePoint->z;
                    //utility::LogInfo("LAS/LAZ::::Using common base point {}, {}, {}", pBasex, pBasey, pBasez);
                }
            }

            px = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
            py = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
            pz = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

            o3dPoint[0] = px - pBasex;
            o3dPoint[1] = py - pBasey;
            o3dPoint[2] = pz - pBasez;

            pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx) >> 8;
            pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx) >> 8;
            pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx) >> 8;

            if ((pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0))
            {
                pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx);
                pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx);
                pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx);
            }

            // if no color use point intensity
            if (pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0)
            {
                double testVal = point_view->getFieldAs<int>(pdal::Dimension::Id::Intensity, idx);

                if (testVal > 0.001)
                {
                    o3dColor[0] = cfactor * testVal;
                    o3dColor[1] = cfactor * testVal;
                    o3dColor[2] = cfactor * testVal;
                }
            }
            else
            {
                o3dColor[0] = (double)(pclPoint.r / 255.f);
                o3dColor[1] = (double)(pclPoint.g / 255.f);
                o3dColor[2] = (double)(pclPoint.b / 255.f);

            }

            pointcloud.points_.push_back(o3dPoint);

            if (o3dColor[0] > 0.001 || o3dColor[1] > 0.001 || o3dColor[2] > 0.001)
            {
                pointcloud.colors_.push_back(o3dColor);
            }

            pointCount++;
        }
    }

    return pointsInFile;
}

UINT LoadPLYToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud)
{
    bool baseSet = false;
    double px = 0.0, py = 0.0, pz = 0.0;
    double pBasex = 0.0, pBasey = 0.0, pBasez = 0.0;
    int pointCount = 0;
    // color from point intensity factor
    double cfactor = 255.0 / 65536.0;
    uint64_t pointsInFile = 0;
    pcl::PointXYZRGB pclPoint, point;
    int pointStep = 1;
    int pointToPixel = 750000;
    PointTable table;
    Eigen::Vector3d o3dPoint, o3dColor;
    Options ops1;
    PlyReader reader;

    ops1.add("filename", fileName.string());
    reader.setOptions(ops1);

    QuickInfo pInfo = reader.preview();

    if (pInfo.m_pointCount < 1)
    {
        utility::LogInfo("Error 1: PLY... NO points {} \n", pointsInFile);
        return 0;
    }

    pointsInFile = pInfo.m_pointCount;

    utility::LogInfo("::::Point cloud file: {}, points:{}", fileName.filename().string(), pointsInFile);

    if (pointsInFile > pointToPixel)
    {
        pointStep = pointsInFile / pointToPixel;
    }

    utility::LogInfo("PLY...Point cloud points {}, thinning factor: {}, \n", pointsInFile, pointStep);

    reader.prepare(table);
    PointViewSet point_views = reader.execute(table);

    if (point_views.size() < 1)
    {
        utility::LogInfo("Error 2: PLY... NO points {} \n", pointsInFile);
        return 0;
    }

    for (PointViewPtr point_view : point_views)
    {
        for (pdal::PointId idx = 0; idx < (point_view->size() - pointStep); idx += pointStep)
        {
            if (!baseSet && (idx == 0))
            {
                pBasex = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                pBasey = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                pBasez = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

                baseSet = true;
            }

            px = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
            py = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
            pz = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

            o3dPoint[0] = px - pBasex;
            o3dPoint[1] = py - pBasey;
            o3dPoint[2] = pz - pBasez;

            pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx) >> 8;
            pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx) >> 8;
            pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx) >> 8;

            if ((pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0))
            {
                pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx);
                pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx);
                pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx);
            }

            // if no color use point intensity
            if (pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0)
            {
                double testVal = point_view->getFieldAs<int>(pdal::Dimension::Id::Intensity, idx);

                o3dColor[0] = (double)testVal * cfactor;
                o3dColor[1] = (double)testVal * cfactor;
                o3dColor[2] = (double)testVal * cfactor;
            }
            else
            {
                o3dColor[0] = (double)(pclPoint.r / 255.f);
                o3dColor[1] = (double)(pclPoint.g / 255.f);
                o3dColor[2] = (double)(pclPoint.b / 255.f);
            }

            pointcloud.points_.push_back(o3dPoint);
            pointcloud.colors_.push_back(o3dColor);

            pointCount++;
        }
    }

    return pointsInFile;
}

UINT LoadPTSToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud)
{
    bool baseSet = false;
    double px = 0.0, py = 0.0, pz = 0.0;
    double pBasex = 0.0, pBasey = 0.0, pBasez = 0.0;
    int pointCount = 0;
    // color from point intensity factor
    double cfactor = 255.0 / 65536.0;
    uint64_t pointsInFile = 0;
    pcl::PointXYZRGB pclPoint, point;
    int pointStep = 1;
    int pointToPixel = 750000;
    PointTable table;
    Eigen::Vector3d o3dPoint, o3dColor;
    Options ops1;
    PtsReader reader;

    ops1.add("filename", fileName.string());
    reader.setOptions(ops1);

    reader.prepare(table);
    PointViewSet point_views = reader.execute(table);

    if (point_views.size() < 1)
    {
        utility::LogInfo("Error 2: PTS... NO points {} \n", pointsInFile);
        return 0;
    }

    pointsInFile = point_views.begin()->get()->size();

    utility::LogInfo("PTS::::Point cloud file: {}, points:{}", fileName.filename().string(), pointsInFile);

    if (pointsInFile > pointToPixel)
    {
        pointStep = pointsInFile / pointToPixel;
    }

    utility::LogInfo("PTS...Point cloud points {}, thinning factor: {}, \n", pointsInFile, pointStep);

    for (PointViewPtr point_view : point_views)
    {
        for (pdal::PointId idx = 0; idx < (point_view->size() - pointStep); idx += pointStep)
        {
            if (!baseSet && (idx == 0))
            {
                pBasex = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
                pBasey = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
                pBasez = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

                baseSet = true;
            }

            px = point_view->getFieldAs<double>(pdal::Dimension::Id::X, idx);
            py = point_view->getFieldAs<double>(pdal::Dimension::Id::Y, idx);
            pz = point_view->getFieldAs<double>(pdal::Dimension::Id::Z, idx);

            o3dPoint[0] = px - pBasex;
            o3dPoint[1] = py - pBasey;
            o3dPoint[2] = pz - pBasez;

            pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx) >> 8;
            pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx) >> 8;
            pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx) >> 8;

            if ((pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0))
            {
                pclPoint.r = point_view->getFieldAs<int>(pdal::Dimension::Id::Red, idx);
                pclPoint.g = point_view->getFieldAs<int>(pdal::Dimension::Id::Green, idx);
                pclPoint.b = point_view->getFieldAs<int>(pdal::Dimension::Id::Blue, idx);
            }

            // if no color use point intensity
            if (pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0)
            {
                double testVal = point_view->getFieldAs<int>(pdal::Dimension::Id::Intensity, idx);

                o3dColor[0] = (double)testVal * cfactor;
                o3dColor[1] = (double)testVal * cfactor;
                o3dColor[2] = (double)testVal * cfactor;
            }
            else
            {
                o3dColor[0] = (double)(pclPoint.r / 255.f);
                o3dColor[1] = (double)(pclPoint.g / 255.f);
                o3dColor[2] = (double)(pclPoint.b / 255.f);
            }

            pointcloud.points_.push_back(o3dPoint);
            pointcloud.colors_.push_back(o3dColor);

            pointCount++;
        }
    }

    return pointsInFile;
}

UINT LoadXYZToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud)
{
    return 0;
}



}

#pragma warning(pop)
