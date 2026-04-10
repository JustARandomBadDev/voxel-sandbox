cmake_minimum_required(VERSION 3.24...4.3)
project(voxel_sandbox LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------------
# Option: use local voxel engine
# -----------------------------------
option(VOXEL_ENGINE_USE_LOCAL "Use local voxel-engine instead of FetchContent" OFF)

set(VOXEL_ENGINE_LOCAL_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../voxel-engine"
    CACHE PATH "Path to local voxel-engine"
)

if(VOXEL_ENGINE_USE_LOCAL AND EXISTS "${VOXEL_ENGINE_LOCAL_PATH}/CMakeLists.txt")
    message(STATUS "Using local voxel-engine at: ${VOXEL_ENGINE_LOCAL_PATH}")
    add_subdirectory(${VOXEL_ENGINE_LOCAL_PATH} voxel-engine-build)

else()
    message(STATUS "Using voxel-engine from GitHub")

    include(FetchContent)

    FetchContent_Declare(
        voxel_engine
        GIT_REPOSITORY https://github.com/JustARandomBadDev/voxel-engine.git
        GIT_TAG main
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(voxel_engine)
endif()

# -----------------------------------
# Sandbox executable
# -----------------------------------
add_executable(voxel_sandbox
    src/main.cpp
    src/world/perlin_noise.cpp
)

target_include_directories(voxel_sandbox
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(voxel_sandbox
    PRIVATE
        voxel_engine
)

get_target_property(VOXEL_ENGINE_TARGET_SOURCE_DIR voxel_engine SOURCE_DIR)

target_compile_definitions(voxel_sandbox
    PRIVATE
        VOXEL_ENGINE_SOURCE_DIR="${VOXEL_ENGINE_TARGET_SOURCE_DIR}"
)
