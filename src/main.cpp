#include <Starry.h>

int main() {
    Starry::Application::setManagerExitRights(true);
    Starry::Application::enableFileLogging(true);

    Starry::Application app;
    app.run();

    if (app.hasFatalError()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}