#pragma once

#pragma warning(push)
#pragma warning(disable: 4201 4324 4263 4264 4265 4266 4267 4512 4996 4702 4244 4250 4068)

#include <map>
#include "open3d/Open3D.h"

using namespace open3d;
using namespace open3d::visualization;
using namespace open3d::geometry;


class EditWindowViewControl : public ViewControlWithEditing
{
public:

    EditWindowViewControl() {};

    void Scale(double scale) override;

    double ZOOM_STEP = 0.01;
    double ZOOM_MIN = 0.01;
    double ZOOM_MAX = 2.0;

 };

class EditWindow : public VisualizerWithEditing
{
    typedef std::pair<int, std::function<bool(Visualizer*)>> KeyCallbackPair;

public:

    EditWindow();

    void RegisterKeyCallback(int key, std::function<bool(Visualizer*)> callback);
    void RegisterKeyActionCallback(int key, std::function<bool(Visualizer*, int, int)> callback);

    void KeyPressCallback(GLFWwindow* window,
                          int key,
                          int scancode,
                          int action,
                          int mods) override;

    bool InitViewControl() override;

    void MouseScrollCallback(GLFWwindow* window, double x, double y) override;
    void MouseMoveCallback(GLFWwindow* window, double x, double y) override;
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) override;
    void Render(bool render_screen) override;

    std::shared_ptr<EditWindowViewControl> m_editWindowControl;

private:
    std::map<int, std::function<bool(Visualizer*)>> key_to_callback_;
    std::map<int, std::function<bool(Visualizer*, int, int)>>  key_action_to_callback_;
};

#pragma warning(pop)