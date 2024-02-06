#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>
#include <fstream>
#include <regex>

#pragma warning(push)
#pragma warning(disable: 4201 4324 4263 4264 4265 4266 4267 4512 4996 4702 4244 4250 4068)

#include "open3d/Open3D.h"
#include "open3d/utility/Timer.h"
#include "tinyfiledialogs.h"
#include "EditWindow.h"
#include "Tools/PointCloudCommon.h"
#include "Tools/PointCloudIO.h"

#include <tbb/tbb.h>
#include "tbb/concurrent_vector.h"
#include "tbb/concurrent_unordered_map.h"

using namespace open3d;

class CloudWranglerApp
{
public:

    bool InitApp();
    bool GetFilesToLoad(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& filesToLoad);

    bool OnEKeyClicked(visualization::Visualizer* vis);
    bool OnLKeyClicked(visualization::Visualizer* vis);
    bool OnOKeyClicked(visualization::Visualizer* vis);
    bool OnSpacebarClicked(visualization::Visualizer* vis);

    int Run3D(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& fileNameList);

    int width = 1600, height = 900, left = 50, top = 50;
    EditWindow CWVisualizerWindow;
    std::vector<std::shared_ptr<const geometry::Geometry>> scene_geometry_ptrs;
    std::shared_ptr<geometry::PointCloud> scene_cloud_ptr;

    NCraftPointCloudTools::AppSettings G_AppSettings;
    std::filesystem::path G_AppPath;
    std::filesystem::path G_LogFilePath;

protected:

private:

};

#pragma warning(pop)