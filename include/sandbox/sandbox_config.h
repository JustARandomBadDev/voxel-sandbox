#ifndef SANDBOX_CONFIG_H
#define SANDBOX_CONFIG_H

enum class SandboxGenerationMode {
    Static,
    Dynamic
};

struct SandboxConfig {
    SandboxGenerationMode generationMode = SandboxGenerationMode::Static;
    int staticSize = 8;
    int dynamicSizeHorizontal = 15;
    int dynamicSizeVertical = 15;
    int dynamicUnloadDistanceHorizontal = 16;
    int dynamicUnloadDistanceVertical = 16;
};

constexpr const char* toString(SandboxGenerationMode p_mode) {
    switch (p_mode) {
        case SandboxGenerationMode::Static:
            return "static";
        case SandboxGenerationMode::Dynamic:
            return "dynamic";
    }

    return "unknown";
}

#endif
