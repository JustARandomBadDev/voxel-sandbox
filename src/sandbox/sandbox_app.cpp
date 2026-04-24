#include "sandbox/sandbox_app.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;
constexpr const char* kWindowTitle = "Voxel Sandbox";

constexpr glm::vec3 kInitialCameraPosition = {0.0f, 10.0f, 0.0f};
constexpr float kCameraFov = 70.0f;
constexpr float kCameraNearPlane = 0.1f;
constexpr float kCameraFarPlane = 1000.0f;
constexpr float kCameraMoveSpeed = 10.0f;
constexpr float kCameraMouseSensitivity = 0.1f;

template<typename Func>
double timeOf(Func func, const std::string& msg) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << msg << elapsed.count() << " ms" << std::endl;
    return elapsed.count();
}

} // namespace

SandboxApp::SandboxApp(SandboxConfig p_config)
: _config(p_config),
  _camera_controller(kCameraMoveSpeed, kCameraMouseSensitivity),
  _world_streamer(_config) {}

SandboxApp::~SandboxApp() {
    shutdown();
}

void SandboxApp::init() {
    initWindow();
    initEngine();

    _camera = Camera(
        kInitialCameraPosition,
        kCameraFov,
        _engine.getAspectRatio(),
        kCameraNearPlane,
        kCameraFarPlane
    );

    logStartupConfig();
    bootstrapWorld();

    _last_frame_time = std::chrono::high_resolution_clock::now();
    _fps_window_start = _last_frame_time;
    _frame_count = 0;
    _initialized = true;
}

void SandboxApp::initWindow() {
    if (glfwInit() != GLFW_TRUE) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(kWindowWidth, kWindowHeight, kWindowTitle, nullptr, nullptr);
    if (_window == nullptr) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

VoxelEngineInitConfig SandboxApp::makeInitConfig() const {
    return {
        .vulkanHost = {
            .requiredInstanceExtensions = getRequiredInstanceExtensions(),
            .createSurface = [this](VkInstance instance, VkSurfaceKHR& surface) {
                return glfwCreateWindowSurface(instance, _window, nullptr, &surface);
            },
            .getFramebufferExtent = [this]() {
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(_window, &width, &height);
                return VkExtent2D{
                    static_cast<uint32_t>(std::max(width, 0)),
                    static_cast<uint32_t>(std::max(height, 0))
                };
            }
        },
        .graphicsResources = {
            .terrainTexture = "assets/textures/terrain.png",
            .voxelVertexShader = "assets/shaders/vert.spv",
            .voxelFragmentShader = "assets/shaders/frag.spv",
        }
    };
}

std::vector<std::string> SandboxApp::getRequiredInstanceExtensions() const {
    uint32_t extension_count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&extension_count);
    if (extensions == nullptr || extension_count == 0) {
        throw std::runtime_error("GLFW did not provide required Vulkan instance extensions");
    }

    return std::vector<std::string>(extensions, extensions + extension_count);
}

void SandboxApp::initEngine() {
    _engine.init(makeInitConfig());
}

void SandboxApp::bootstrapWorld() {
    double total = 0.0;

    total += timeOf([&]() { _world_streamer.syncInitial(_engine, _camera); }, "Temps de génération initiale des chunks : ");

    total += timeOf([&]() { _engine.update(_camera); }, "Temps de mise à jour moteur : ");

    std::cout << "Temps total : " << total << " ms" << std::endl;
}

void SandboxApp::logStartupConfig() const {
    std::cout << "Sandbox generation mode: " << toString(_config.generationMode) << std::endl;

    if (_config.generationMode == SandboxGenerationMode::Static) {
        std::cout << "Static chunk distance: " << _config.staticSize << " chunks" << std::endl;
        return;
    }

    std::cout << "Dynamic load distance xz: " << _config.dynamicSizeHorizontal << " chunks" << std::endl;
    std::cout << "Dynamic load distance y: " << _config.dynamicSizeVertical << " chunks" << std::endl;
    std::cout << "Dynamic unload distance xz: " << _config.dynamicUnloadDistanceHorizontal << " chunks" << std::endl;
    std::cout << "Dynamic unload distance y: " << _config.dynamicUnloadDistanceVertical << " chunks" << std::endl;
}

SandboxApp::FrameStats SandboxApp::updateFrameStats() {
    const auto now = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> frame_delta = now - _last_frame_time;
    _last_frame_time = now;

    _frame_count++;

    FrameStats stats;
    stats.deltaTimeSeconds = frame_delta.count();

    const std::chrono::duration<double, std::milli> fps_elapsed = now - _fps_window_start;
    if (fps_elapsed.count() >= 1000.0) {
        stats.shouldReportFps = true;
        stats.fps = _frame_count;
        _frame_count = 0;
        _fps_window_start = now;
    }

    return stats;
}

void SandboxApp::update() {
    const FrameStats frame_stats = updateFrameStats();
    if (frame_stats.shouldReportFps) {
        std::cout << "\rFPS: " << frame_stats.fps << " " << std::flush;
    }

    glfwPollEvents();
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(_window, GLFW_TRUE);
    }

    _camera.updateProjection(_engine.getAspectRatio());
    _camera_controller.update(*_window, _camera, frame_stats.deltaTimeSeconds);
    if (_config.generationMode == SandboxGenerationMode::Dynamic) {
        _world_streamer.update(_engine, _camera);
    }
    _engine.update(_camera);
}

void SandboxApp::render() {
    _engine.render(_camera);
}

void SandboxApp::run() {
    if (!_initialized) {
        throw std::runtime_error("SandboxApp::run() -> app must be initialized before run()");
    }

    while (!glfwWindowShouldClose(_window)) {
        update();
        render();
    }
}

void SandboxApp::shutdown() {
    if (_initialized) {
        _engine.shutdown();
        _initialized = false;
    }

    if (_window != nullptr) {
        glfwDestroyWindow(_window);
        _window = nullptr;
    }

    glfwTerminate();
}
