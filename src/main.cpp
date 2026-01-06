#include "Application.h"

int main() {
    const Editor::ApplicationConfig config;

	Editor::Application::setConfig(config);

    Editor::Application app;
    app.run();

    if (app.hasFatalError()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}