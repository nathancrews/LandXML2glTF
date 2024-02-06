#include "framework.h"
#include "CloudWranglerApp.h"

CloudWranglerApp CWApp;

void print_fcn(const std::string& logString)
{
    std::fstream fs;
    fs.open(CWApp.G_LogFilePath, std::fstream::out | std::fstream::app);

    if (fs.is_open())
    {
        fs << logString;
        fs << "\n";

        fs.flush();
        fs.close();
    }
}

int main(int argc, char* argv[])
{
    if (argc > 0)
    {
        CWApp.G_AppPath = argv[0];
        CWApp.G_AppPath = CWApp.G_AppPath.remove_filename();

        CWApp.G_LogFilePath = CWApp.G_AppPath;

        CWApp.G_LogFilePath.replace_filename("1_CloudWrangler");
        CWApp.G_LogFilePath.replace_extension("log");
    }

    utility::Logger::GetInstance().SetPrintFunction(print_fcn);

    utility::SetVerbosityLevel(utility::VerbosityLevel::Debug);
    utility::LogInfo("Open3D {}\n", OPEN3D_VERSION);

    tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage> filesToLoad;

    CWApp.InitApp();

    CWApp.GetFilesToLoad(filesToLoad);

    CWApp.Run3D(filesToLoad);

    return 0;
}

bool CloudWranglerApp::GetFilesToLoad(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& filesToLoad)
{
    bool retval = false;

    const char* extentions[3] = { "*.pcd", "*.las", "*.laz" };

    char const* fnstr = tinyfd_openFileDialog("Point cloud to load", "", 3,
                                              extentions, "point cloud files", 1);

    if (fnstr)
    {
        std::string fileNames = fnstr;

        std::regex reg("\\|");
        std::sregex_token_iterator iter(fileNames.begin(), fileNames.end(), reg, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> fileNameList(iter, end);

        std::vector<std::filesystem::path> pcdFileNameList;
        std::filesystem::path fileName;

        for (std::string fname : fileNameList)
        {
            fileName = fname;

            NCraftPointCloudTools::FileProcessPackage toAddResult(fileName);

            uintmax_t fsize = 0;
            __std_win_error wep = std::filesystem::_File_size(fileName, fsize);

            toAddResult.m_fileSize = fsize;
            toAddResult.m_modelType = 0;
            toAddResult.m_imageFileCacheOk = false;

            filesToLoad.push_back(toAddResult);

            retval = true;
        }
    }

    return retval;
}

bool CloudWranglerApp::OnSpacebarClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("[SPACEBAR]");

    bool hasNormals = scene_cloud_ptr->HasNormals();

//    std::tuple<std::shared_ptr<TriangleMesh>, std::vector<size_t>> hullMeshes = NCraftPointCloudTools::scene_cloud_ptr->ComputeConvexHull(false);

 //   if (std::get<0>(hullMeshes))
    {
        //MainvisualizerWindow.ClearGeometries();

        //MainvisualizerWindow.AddGeometry(std::get<0>(hullMeshes));

        CWVisualizerWindow.GetRenderOption().point_show_normal_ = true;
        CWVisualizerWindow.UpdateRender();
        
    }

    return true;
}

bool CloudWranglerApp::OnLKeyClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("Loading new files");

    tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage> filesToLoad;

    if (!GetFilesToLoad(filesToLoad))
    {
        return true;
    }

    visualization::ViewParameters OrigState;
    CWVisualizerWindow.GetViewControl().ConvertToViewParameters(OrigState);

    utility::Timer timer;
    double exeTime = 0.0;
    bool sceneNotEmpty = false;

    timer.Start();
    UINT totalPoints = NCraftPointCloudTools::LoadPointCloudFilesParallel(filesToLoad, &G_AppSettings.mergedBasePoint);
    timer.Stop();

    exeTime = timer.GetDurationInSecond();

    utility::LogInfo("Loaded {} Points in {} seconds", totalPoints, exeTime);

    if (CWVisualizerWindow.HasGeometry())
    {
        sceneNotEmpty = true;
        CWVisualizerWindow.ClearGeometries();
    }

    scene_geometry_ptrs.clear();

    if (NCraftPointCloudTools::PackToSingleCloud(filesToLoad, scene_cloud_ptr) > 0)
    {
        scene_geometry_ptrs.push_back(scene_cloud_ptr);
    }

    for (const auto& geometry_ptr : scene_geometry_ptrs)
    {
        Visualizer* parent = dynamic_cast<Visualizer*>(&CWVisualizerWindow);
        if (parent)
        {
            parent->AddGeometry(geometry_ptr, true);
        }
        else if (!CWVisualizerWindow.AddGeometry(geometry_ptr, true))
        {
            utility::LogWarning("[DrawGeometriesWithEditing] Failed adding geometry.");
        }
    }

    CWVisualizerWindow.UpdateRender();

    if (sceneNotEmpty == true)
    {
        CWVisualizerWindow.GetViewControl().ConvertFromViewParameters(OrigState);
    }

    return true;
}

bool CloudWranglerApp::OnEKeyClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("Clearing scene");

    G_AppSettings.mergedBasePoint = { 0.0f,0.0f,0.0f };

    scene_cloud_ptr->Clear();
    scene_geometry_ptrs.clear();

    CWVisualizerWindow.ClearGeometries();
    CWVisualizerWindow.UpdateRender();
    CWVisualizerWindow.GetViewControl().Reset();

    return true;
}

bool CloudWranglerApp::OnOKeyClicked(visualization::Visualizer* vis)
{
    utility::LogInfo("Applying outlier removal filter");

    visualization::ViewParameters OrigState;
    CWVisualizerWindow.GetViewControl().ConvertToViewParameters(OrigState);

    if (CWVisualizerWindow.HasGeometry())
    {
        CWVisualizerWindow.ClearGeometries();
    }

    scene_geometry_ptrs.clear();

    size_t neighbors = 20;
    double distRatio = 3.0;

    NCraftPointCloudTools::RemoveOutliers(neighbors, distRatio, scene_cloud_ptr);

    scene_geometry_ptrs.push_back(scene_cloud_ptr);

    for (const auto& geometry_ptr : scene_geometry_ptrs)
    {
        if (!CWVisualizerWindow.AddGeometry(geometry_ptr, true))
        {
            utility::LogWarning("Failed Re-Adding geometry.");
        }
    }

    CWVisualizerWindow.UpdateRender();

    CWVisualizerWindow.GetViewControl().ConvertFromViewParameters(OrigState);

    return true;
}

bool OnSpacebarClicked(visualization::Visualizer* vis)
{
    return CWApp.OnSpacebarClicked(vis);
}

bool OnEKeyClicked(visualization::Visualizer* vis)
{
    return CWApp.OnEKeyClicked(vis);
}

bool OnLKeyClicked(visualization::Visualizer* vis)
{
    return CWApp.OnLKeyClicked(vis);
}

bool OnOKeyClicked(visualization::Visualizer* vis)
{
    return CWApp.OnOKeyClicked(vis);
}

bool CloudWranglerApp::InitApp()
{
    scene_cloud_ptr = std::make_shared<geometry::PointCloud>();

    if (!CWVisualizerWindow.CreateVisualizerWindow("Cloud Wrangler", width, height, left,
                                                     top))
    {
        utility::LogWarning(
            "[CWVisualizerWindow] Failed creating OpenGL "
            "window.");
        return false;
    }

    std::map<int, std::function<bool(visualization::Visualizer*)>> keys_to_callback;
    keys_to_callback[GLFW_KEY_SPACE] = ::OnSpacebarClicked;
    keys_to_callback[GLFW_KEY_E] = ::OnEKeyClicked;
    keys_to_callback[GLFW_KEY_L] = ::OnLKeyClicked;
    keys_to_callback[GLFW_KEY_O] = ::OnOKeyClicked;

    for (auto key_func_pair : keys_to_callback)
    {
        CWVisualizerWindow.RegisterKeyCallback(key_func_pair.first,
                                               key_func_pair.second);
    }

    CWVisualizerWindow.GetRenderOption().SetPointSize(3.0);
    CWVisualizerWindow.GetRenderOption().show_coordinate_frame_ = true;
    CWVisualizerWindow.GetRenderOption().background_color_ = Eigen::Vector3d(0.25f, 0.25f, 0.25f);
    CWVisualizerWindow.GetViewControl().ChangeFieldOfView(90.0);

    return true;
}

int CloudWranglerApp::Run3D(tbb::concurrent_vector<NCraftPointCloudTools::FileProcessPackage>& fileNameList)
{
    utility::Timer timer;
    double exeTime = 0.0;

    timer.Start();
    UINT totalPoints = NCraftPointCloudTools::LoadPointCloudFilesParallel(fileNameList, &G_AppSettings.mergedBasePoint);
    timer.Stop();

    exeTime = timer.GetDurationInSecond();

    utility::LogInfo("Loaded {} Points in {} seconds", totalPoints, exeTime);

    std::vector<std::shared_ptr<const geometry::Geometry>> geometry_ptrs;

    if (NCraftPointCloudTools::PackToSingleCloud(fileNameList, scene_cloud_ptr) > 0)
    {
        geometry_ptrs.push_back(scene_cloud_ptr);
    }

    for (const auto& geometry_ptr : geometry_ptrs)
    {
        if (!CWVisualizerWindow.AddGeometry(geometry_ptr, true))
        {
            utility::LogWarning(
                "[DrawGeometriesWithEditing] Failed adding geometry.");
            utility::LogWarning(
                "[DrawGeometriesWithEditing] Possibly due to bad "
                "geometry "
                "or wrong geometry type.");
        }
    }

    CWVisualizerWindow.Run();
    CWVisualizerWindow.DestroyVisualizerWindow();

    return 0;
}


