#include <cstdlib>
#include <exception>
#include <iostream>

#include "sandbox/sandbox_app.h"

int main() {
    try {
        SandboxApp app;
        app.init();
        app.run();
        app.shutdown();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
