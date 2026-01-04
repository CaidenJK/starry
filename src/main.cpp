#include "Application.h"

int main() {
    Editor::Application::setManagerExitRights(true);
    Editor::Application::enableFileLogging(false);

    Editor::Application app;
    app.run();

    if (app.hasFatalError()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}