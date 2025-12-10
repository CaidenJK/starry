#include <Starry.h>

int main() {
    Starry::Application app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (app.hasFatalError()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}