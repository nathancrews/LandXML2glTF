#include "framework.h"
#include "RenderToImage.h"
#include <iostream>
#include <memory>
#include <thread>
#include <fstream>

#pragma warning(push)
#pragma warning(disable: 4201 4324 4263 4264 4265 4266 4267 4512 4996 4702 4244 4250 4068)

#include "open3d/Open3D.h"
#include "open3d/utility/Timer.h"

using namespace open3d;

int Run3D(std::vector<std::filesystem::path>& pcdFileNameList);

std::filesystem::path G_AppPath;

void print_fcn(const std::string& logString)
{
    std::filesystem::path logFilePath = G_AppPath;

    logFilePath.replace_filename("1_imageGen");
    logFilePath.replace_extension("log");

    std::fstream fs;
    fs.open(logFilePath, std::fstream::out | std::fstream::app);

    fs << logString;
    fs << "\n";

    fs.flush();
    fs.close();
}


int main(int argc, char* argv[])
{
//    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);

    FreeConsole();

    std::filesystem::path appPath = argv[0];
    appPath = appPath.remove_filename();

    G_AppPath = appPath;
    utility::Logger::GetInstance().SetPrintFunction(print_fcn);

    std::filesystem::path filePath;

    if (argc > 1)
    {
        filePath = argv[1];
    }

    utility::Timer timer;

    timer.Start();
    RenderToImage(appPath, filePath);
    timer.Stop();

    double exeTime = timer.GetDurationInSecond();

    utility::LogInfo("Total Duration: {} seconds\n", exeTime);

    return 0;
}

#if 0
int oldMain(int argc, char* argv[])
{
    utility::SetVerbosityLevel(utility::VerbosityLevel::Debug);
    if (argc < 1)
    {
        utility::LogInfo("Open3D {}\n", OPEN3D_VERSION);
        utility::LogInfo("\n");
        utility::LogInfo("Usage:\n");
        utility::LogInfo("    > TestVisualizer [mesh|pointcloud] [filename]\n");
        // CI will execute this file without input files, return 0 to pass
        return 0;
    }

    std::filesystem::path appPath = argv[0];
    appPath = appPath.remove_filename();
    char const* fnstr;

    if (argc > 1)
    {
        fnstr = argv[1];
    }
    else
    {
        const char* extentions[3] = { "*.pcd", "*.las", "*.laz" };

        fnstr = tinyfd_openFileDialog("Point cloud to load", "", 3,
                                      extentions, "point cloud files", 1);
    }

    if (fnstr)
    {
        std::string fileNames = fnstr;

        std::regex reg("\\|");
        std::sregex_token_iterator iter(fileNames.begin(), fileNames.end(), reg, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> fileNameList(iter, end);

        std::vector<std::filesystem::path> pcdFileNameList;
        std::filesystem::path fileName;

        pcl::PointXYZRGB pclPoint, point;
        double cfactor = 255.0 / 65536.0;
        double px = 0.0, py = 0.0, pz = 0.0;
        double pBasex = 0.0, pBasey = 0.0, pBasez = 0.0;
        bool baseSet = false;

        for (std::string fname : fileNameList)
        {
            fileName = fname;

            if (fileName.extension() == ".las" || fileName.extension() == ".laz")
            {
                PointTable table;

                Options ops1;
                ops1.add("filename", fileName.string());
                LasReader reader;
                reader.setOptions(ops1);

                open3d::geometry::PointCloud o3dCloud;
                Eigen::Vector3d o3dPoint, o3dColor;

                pcl::PointCloud<pcl::PointXYZRGB>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZRGB>);

                const LasHeader lsHeader = reader.header();

                reader.prepare(table);
                PointViewSet point_views = reader.execute(table);

                for (PointViewPtr point_view : point_views)
                {
                    for (pdal::PointId idx = 0; idx < point_view->size();)
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

                        point.x = px - pBasex;
                        point.y = py - pBasey;
                        point.z = pz - pBasez;

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

                        // if no color load intensities
                        if (pclPoint.r == 0 && pclPoint.g == 0 && pclPoint.b == 0)
                        {
                            int testVal = point_view->getFieldAs<int>(pdal::Dimension::Id::Intensity, idx);

                            pclPoint.r = testVal * cfactor;
                            pclPoint.g = testVal * cfactor;
                            pclPoint.b = testVal * cfactor;
                        }

                        o3dColor[0] = pclPoint.r / 255;
                        o3dColor[1] = pclPoint.g / 255;
                        o3dColor[2] = pclPoint.b / 255;

                        pclPoint.x = point.x;
                        pclPoint.y = point.y;
                        pclPoint.z = point.z;

                        cloud->points.push_back(pclPoint);

                        idx++;
                    }
                }

                std::filesystem::path pcdFile = fileName;

                pcdFile.replace_extension("pcd");

                pcl::io::savePCDFileBinary(pcdFile.string(), *cloud);

                fileName = pcdFile;
            }

            pcdFileNameList.push_back(fileName);
        }

        Run3D(pcdFileNameList);
    }

    return 0;
}

bool OnSpacebarKeyClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("[SPACEBAR]");

    return true;
}

bool OnNKeyClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("Loading new files");


    return true;
}

int Run3D(std::vector<std::filesystem::path>& pcdFileNameList)
{
    int width = 1600, height = 900, left = 50, top = 50;
    visualization::VisualizerWithEditing visualizer;

    std::vector<std::shared_ptr<const geometry::Geometry>> geometry_ptrs;
    auto cloud_ptr = std::make_shared<geometry::PointCloud>();

    for (std::filesystem::path file_name : pcdFileNameList)
    {
        auto new_cloud_ptr = std::make_shared<geometry::PointCloud>();

        if (io::ReadPointCloud(file_name.string(), *new_cloud_ptr))
        {
            *cloud_ptr += *new_cloud_ptr;

            utility::LogInfo("Successfully read {}\n", file_name.string());
        }
        else
        {
            utility::LogWarning("Failed to read {}\n\n", file_name.string());
            return 1;
        }
    }

    geometry_ptrs.push_back(cloud_ptr);

    auto& app = visualization::gui::Application::GetInstance();

    std::map<int, std::function<bool(visualization::Visualizer*)>> keys_to_callback;
    keys_to_callback[GLFW_KEY_SPACE] = OnSpacebarKeyClicked;
    keys_to_callback[GLFW_KEY_N] = OnNKeyClicked;

    if (!visualizer.CreateVisualizerWindow("Cloud Wrangler", width, height, left,
                                           top))
    {
        utility::LogWarning(
            "[DrawGeometriesWithEditing] Failed creating OpenGL "
            "window.");
        return false;
    }

    for (const auto& geometry_ptr : geometry_ptrs)
    {
        if (!visualizer.AddGeometry(geometry_ptr, true))
        {
            utility::LogWarning(
                "[DrawGeometriesWithEditing] Failed adding geometry.");
            utility::LogWarning(
                "[DrawGeometriesWithEditing] Possibly due to bad "
                "geometry "
                "or wrong geometry type.");
        }
    }

    for (auto key_func_pair : keys_to_callback)
    {
        visualizer.RegisterKeyCallback(key_func_pair.first,
                                       key_func_pair.second);
    }

    visualizer.Run();
    visualizer.DestroyVisualizerWindow();

    return 0;
}

#endif

#pragma warning(pop)
