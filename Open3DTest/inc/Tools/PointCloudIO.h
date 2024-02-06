#pragma once
#include "Tools/PointCloudCommon.h"

namespace pcl
{
    struct PointXYZRGB;
}

namespace NCraftPointCloudTools
{

UINT PackToSingleCloud(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& filesToLoad,
                       std::shared_ptr<geometry::PointCloud>& cloud_ptr, bool RemoveOutliers=false);

UINT LoadLASorLAZToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud, 
                            pcl::PointXYZRGB* pCommonBasePoint);
UINT LoadPLYToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud);
UINT LoadPTSToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud);
UINT LoadXYZToO3DCloud(std::filesystem::path& fileName, geometry::PointCloud& pointcloud);

UINT LoadPointCloudFile(NCraftPointCloudTools::FileProcessPackage& outLoadResults, pcl::PointXYZRGB* pCommonBasePoint);
UINT LoadPointCloudFilesParallel(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& outLoadResults, 
                                 pcl::PointXYZRGB* mergedBasePoint);

}

