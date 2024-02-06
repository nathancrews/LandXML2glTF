#include "EditWindow.h"


void EditWindowViewControl::Scale(double scale)
{
    //double zoomInc = (ZOOM_STEP);
    //zoom_ = std::max(std::min(zoom_ + scale * zoomInc, ZOOM_MAX), ZOOM_MIN);
    //SetProjectionParameters();

    ViewControlWithEditing::Scale(scale);
}

EditWindow::EditWindow()
{
    
}

bool EditWindow::InitViewControl()
{
    return VisualizerWithEditing::InitViewControl();
}

void EditWindow::Render(bool render_screen)
{

    VisualizerWithEditing::Render(render_screen);
}

void EditWindow::MouseScrollCallback(GLFWwindow* window, double x, double y)
{
    // reverse scroll wheel zoom
    view_control_ptr_->Scale(-y);
    is_redraw_required_ = true;
}

void EditWindow::MouseMoveCallback(GLFWwindow* window, double x, double y)
{

    VisualizerWithEditing::MouseMoveCallback(window, x, y);
}

void EditWindow::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    VisualizerWithEditing::MouseButtonCallback(window, button, action, mods);
}

void EditWindow::KeyPressCallback(GLFWwindow* window, int key, int scancode,
                                  int action, int mods)
{
    bool handled = false;

    if (action == GLFW_RELEASE)
    {
        auto callback = key_to_callback_.find(key);
        if (callback != key_to_callback_.end())
        {
            if (callback->second(this))
            {
                handled = true;
                UpdateGeometry();
                UpdateRender();
            }
        }
    }

    if (!handled)
    {
        VisualizerWithEditing::KeyPressCallback(window, key, scancode, action, mods);
    }
}

void EditWindow::RegisterKeyCallback(int key, std::function<bool(Visualizer*)> callback)
{
    key_to_callback_[key] = callback;
}

void EditWindow::RegisterKeyActionCallback(int key, std::function<bool(Visualizer*, int, int)> callback)
{
    key_action_to_callback_[key] = callback;
}

