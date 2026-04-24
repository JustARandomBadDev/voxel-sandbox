# voxel-sandbox

## Project Overview

`voxel-sandbox` is a small client application built on top of
`voxel-engine`.

It is not a game. It is a test and debugging environment used to validate
engine behavior such as:

- procedural chunk generation
- chunk loading and unloading
- meshing and rendering
- camera movement and basic runtime interaction

The project is intentionally simple and exists to exercise the engine in a
controlled environment.

## Features

- Static world generation
- Dynamic generation around the player/camera
- Rendering through `voxel-engine`
- Simple debug and experimentation environment

## Requirements

- Linux
- C++20-compatible compiler
- CMake 3.24 or newer
- Vulkan development environment available on the system
- A graphics driver with Vulkan support

Dependencies:

- `voxel-engine` is pulled automatically through CMake `FetchContent`
  by default
- `GLFW` is also pulled through CMake `FetchContent` if not already present

## Build Instructions

Debug build:

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

Release build:

```bash
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

Optional: use a local checkout of `voxel-engine` instead of FetchContent:

```bash
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Debug \
  -DVOXEL_ENGINE_USE_LOCAL=ON \
  -DVOXEL_ENGINE_LOCAL_PATH=../voxel-engine
cmake --build build
```

## Running the Sandbox

After building, run:

```bash
./build/voxel_sandbox
```

Supported runtime arguments:

- `--generation static`
- `--generation dynamic`
- `--static-size <value>`
- `--size <value>` as an alias for `--static-size`
- `--size-xz <value>` for dynamic horizontal chunk distance
- `--size-y <value>` for dynamic vertical chunk distance

Examples:

Default startup:

```bash
./build/voxel_sandbox
```

Static generation with a larger fixed area:

```bash
./build/voxel_sandbox --generation static --static-size 16
```

Dynamic generation around the camera:

```bash
./build/voxel_sandbox --generation dynamic --size-xz 15 --size-y 15
```

## Controls

- `W` / `A` / `S` / `D`: move horizontally
- `Space`: move down
- `C`: move up
- `Mouse`: look around
- `Left Shift`: faster movement
- `Esc`: close the application

## Project Structure

- `src/`: application entry point and sandbox runtime code
- `include/`: sandbox headers
- `assets/`: shaders and textures used by the sandbox
- `CMakeLists.txt`: build configuration

## Relationship with voxel-engine

`voxel-engine` is the core library. It provides the underlying voxel systems,
rendering pipeline, and chunk management used by this application.

`voxel-sandbox` is only a client of that engine. Its role is to drive the
engine, expose test scenarios, and make engine behavior easier to validate.

## Notes / Limitations

- This project is not production-ready
- It is not intended to be a standalone product
- It prioritizes testing and iteration over polish
- Performance characteristics may vary depending on the chosen generation mode
- The code is meant to stay easy to modify for experiments and debugging

## Future Improvements

- Add more focused engine validation scenarios
- Improve runtime diagnostics and debug output
- Add a few reproducible profiling configurations
