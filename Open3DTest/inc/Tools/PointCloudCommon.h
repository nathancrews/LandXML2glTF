#pragma once
#include "framework.h"
#include <iostream>
#include <memory>
#include <thread>
#include <filesystem>

#ifdef NCRAFTPOINTCLOUDAPI_EXPORT
#define NCRAFTPOINTCLOUDAPI __declspec(dllexport)
#elif defined(NCRAFTIMAGEGENAPI_IMPORT)
#define NCRAFTPOINTCLOUDAPI __declspec(dllimport)
#else
#define NCRAFTPOINTCLOUDAPI
#endif

#pragma warning(push)
#pragma warning(disable: 4201 4324 4263 4264 4265 4266 4267 4512 4996 4702 4244 4250 4068)

#include "open3d/Open3D.h"
#include "open3d/visualization/rendering/Camera.h"
#include "open3d/visualization/rendering/filament/FilamentEngine.h"
#include "open3d/visualization/rendering/filament/FilamentRenderer.h"
#include "open3d/utility/Timer.h"

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/common.h>

#include <tbb/tbb.h>
#include "tbb/concurrent_vector.h"
#include "tbb/concurrent_unordered_map.h"
using namespace open3d;
using namespace open3d::visualization::gui;
using namespace open3d::visualization::rendering;

namespace NCraftPointCloudTools
{

struct AppSettings
{
    pcl::PointXYZRGB mergedBasePoint = { 0.0f,0.0f,0.0f };
    UINT imageWidth = 1440;
    UINT imageHeight = 1024;
    UINT is_Licensed = false;
    std::string imageFormat = "png";
    std::string licenseKey;
};

struct FileProcessPackage
{
    FileProcessPackage() {};
    FileProcessPackage(std::filesystem::path& filePath) { m_FileName = filePath; };

    bool m_imageFileCacheOk = false;
    UINT m_modelType = 0; // 0 = point cloud, 1 = gltf
    std::filesystem::path  m_FileName;
    std::filesystem::path m_ImageName;
    uintmax_t m_fileSize = 0;
    double m_processTimeSeconds = 0.0;

    uintmax_t m_pointCount = 0;
    std::shared_ptr<geometry::PointCloud> m_cloudPtr;
};

NCRAFTPOINTCLOUDAPI size_t RemoveOutliers(size_t& neighbors, double& distRatio, std::shared_ptr<geometry::PointCloud>& pCloud);

NCRAFTPOINTCLOUDAPI bool ReadAppSettings(std::filesystem::path& appDataPath, AppSettings& outSettings);

NCRAFTPOINTCLOUDAPI UINT GetFileNamesFromDirectory(std::filesystem::path& filePath, std::vector<std::string>& allowedFileExtensions, 
                                                 std::vector<std::filesystem::path>& outDirectoryFilenames);
NCRAFTPOINTCLOUDAPI UINT GetCloudFileNamesFromDirectory(std::filesystem::path& filePath, std::vector<std::string>& allowedFileExtensions,
                                                      std::vector<std::filesystem::path>& outDirectoryFilenames);

NCRAFTPOINTCLOUDAPI extern std::vector<std::string> ModelFileExtensions;
NCRAFTPOINTCLOUDAPI extern std::vector<std::string> PointcloudFileExtensions;

}

#pragma warning(pop)
