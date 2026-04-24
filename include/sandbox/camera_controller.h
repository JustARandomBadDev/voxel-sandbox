#ifndef SANDBOX_CAMERA_CONTROLLER_H
#define SANDBOX_CAMERA_CONTROLLER_H

struct GLFWwindow;
class Camera;

class SandboxCameraController {
public:
    SandboxCameraController(float p_move_speed, float p_mouse_sensitivity);

    void update(GLFWwindow& p_window, Camera& p_camera, float p_delta_time);

private:
    float _move_speed = 0.0f;
    float _mouse_sensitivity = 0.0f;
    bool _has_last_cursor_pos = false;
    double _last_cursor_x = 0.0;
    double _last_cursor_y = 0.0;
};

#endif
