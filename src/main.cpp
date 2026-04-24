#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "sandbox/sandbox_config.h"
#include "sandbox/sandbox_app.h"

namespace {

void printUsage(const char* p_program_name) {
    std::cout
        << "Usage: " << p_program_name << " [options]\n"
        << "  --generation <static|dynamic>\n"
        << "  --static-size <chunks>\n"
        << "  --size <chunks>            Alias for --static-size\n"
        << "  --size-xz <chunks>         Dynamic load distance on X/Z\n"
        << "  --size-y <chunks>          Dynamic load distance on Y\n"
        << "  --help                     Show this help\n";
}

int parseNonNegativeInt(std::string_view p_flag, std::string_view p_value) {
    size_t parsed_size = 0;
    const std::string value_string(p_value);
    const int parsed_value = std::stoi(value_string, &parsed_size);

    if (parsed_size != value_string.size() || parsed_value < 0) {
        throw std::runtime_error("Invalid value for " + std::string(p_flag) + ": " + value_string);
    }

    return parsed_value;
}

SandboxConfig parseArgs(int argc, char** argv) {
    SandboxConfig config;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];

        auto require_value = [&](std::string_view p_flag) -> std::string_view {
            if (i + 1 >= argc) {
                throw std::runtime_error("Missing value for " + std::string(p_flag));
            }

            ++i;
            return argv[i];
        };

        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(EXIT_SUCCESS);
        }

        if (arg == "--generation") {
            const std::string_view value = require_value(arg);
            if (value == "static") {
                config.generationMode = SandboxGenerationMode::Static;
                continue;
            }

            if (value == "dynamic") {
                config.generationMode = SandboxGenerationMode::Dynamic;
                continue;
            }

            throw std::runtime_error("Invalid value for --generation: " + std::string(value));
        }

        if (arg == "--static-size" || arg == "--size") {
            config.staticSize = parseNonNegativeInt(arg, require_value(arg));
            continue;
        }

        if (arg == "--size-xz") {
            config.dynamicSizeHorizontal = parseNonNegativeInt(arg, require_value(arg));
            config.dynamicUnloadDistanceHorizontal = config.dynamicSizeHorizontal + 1;
            continue;
        }

        if (arg == "--size-y") {
            config.dynamicSizeVertical = parseNonNegativeInt(arg, require_value(arg));
            config.dynamicUnloadDistanceVertical = config.dynamicSizeVertical + 1;
            continue;
        }

        throw std::runtime_error("Unknown argument: " + std::string(arg));
    }

    return config;
}

} // namespace

int main(int argc, char** argv) {
    try {
        const SandboxConfig config = parseArgs(argc, argv);
        SandboxApp app(config);
        app.init();
        app.run();
        app.shutdown();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
