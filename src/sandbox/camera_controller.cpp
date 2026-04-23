#include "sandbox/camera_controller.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "core/camera.h"

SandboxCameraController::SandboxCameraController(float p_move_speed, float p_mouse_sensitivity)
: _move_speed(p_move_speed), _mouse_sensitivity(p_mouse_sensitivity) {}

void SandboxCameraController::update(GLFWwindow& p_window, Camera& p_camera, float p_delta_time) {
    const glm::vec3 world_up = p_camera.getWorldUp();
    const glm::vec3 direction = p_camera.getDirection();
    const glm::vec3 camera_right = glm::normalize(glm::cross(direction, world_up));
    const glm::vec3 forward = glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));

    const float axis_x =
        (glfwGetKey(&p_window, GLFW_KEY_D) == GLFW_PRESS ? 1.0f : 0.0f) -
        (glfwGetKey(&p_window, GLFW_KEY_A) == GLFW_PRESS ? 1.0f : 0.0f);
    const float axis_y =
        (glfwGetKey(&p_window, GLFW_KEY_C) == GLFW_PRESS ? 1.0f : 0.0f) -
        (glfwGetKey(&p_window, GLFW_KEY_SPACE) == GLFW_PRESS ? 1.0f : 0.0f);
    const float axis_z =
        (glfwGetKey(&p_window, GLFW_KEY_W) == GLFW_PRESS ? 1.0f : 0.0f) -
        (glfwGetKey(&p_window, GLFW_KEY_S) == GLFW_PRESS ? 1.0f : 0.0f);

    glm::vec3 move_direction = forward * axis_z + world_up * axis_y + camera_right * axis_x;
    if (glm::length(move_direction) > 0.0f) {
        move_direction = glm::normalize(move_direction);
    }

    const float speed_multiplier = glfwGetKey(&p_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 5.0f : 1.0f;
    p_camera.translate(move_direction * _move_speed * speed_multiplier * p_delta_time);

    double cursor_x = 0.0;
    double cursor_y = 0.0;
    glfwGetCursorPos(&p_window, &cursor_x, &cursor_y);

    if (!_has_last_cursor_pos) {
        _last_cursor_x = cursor_x;
        _last_cursor_y = cursor_y;
        _has_last_cursor_pos = true;
    }

    const float look_delta_x = static_cast<float>(cursor_x - _last_cursor_x);
    const float look_delta_y = static_cast<float>(_last_cursor_y - cursor_y);
    _last_cursor_x = cursor_x;
    _last_cursor_y = cursor_y;

    p_camera.rotate(- look_delta_x * _mouse_sensitivity, look_delta_y * _mouse_sensitivity);
}
