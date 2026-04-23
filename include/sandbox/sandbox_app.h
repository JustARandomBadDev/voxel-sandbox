#ifndef SANDBOX_APP_H
#define SANDBOX_APP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <chrono>
#include <string>
#include <vector>

#include "core/camera.h"
#include "engine/voxel_engine.h"
#include "sandbox/camera_controller.h"
#include "sandbox/world_streamer.h"

class SandboxApp {
public:
    SandboxApp();
    ~SandboxApp();

    SandboxApp(const SandboxApp&) = delete;
    SandboxApp& operator=(const SandboxApp&) = delete;

    void init();
    void run();
    void shutdown();

private:
    struct FrameStats {
        float deltaTimeSeconds = 0.0f;
        bool shouldReportFps = false;
        int fps = 0;
    };

    GLFWwindow* _window = nullptr;
    VoxelEngine _engine;
    Camera _camera;
    SandboxCameraController _camera_controller;
    SandboxWorldStreamer _world_streamer;
    bool _initialized = false;

    std::chrono::high_resolution_clock::time_point _last_frame_time;
    std::chrono::high_resolution_clock::time_point _fps_window_start;
    int _frame_count = 0;

    void initWindow();
    void initEngine();
    void bootstrapWorld();
    FrameStats updateFrameStats();
    void update();
    void render();
    VoxelEngineInitConfig makeInitConfig() const;
    std::vector<std::string> getRequiredInstanceExtensions() const;
};

#endif
