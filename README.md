# Starry

## Warnings
### If using MacOS, be sure to set these Enviroment variables before running:

-`export VK_ICD_FILENAMES=path/to/vulkansdk/macOS/share/vulkan/icd.d/MoltenVK_icd.json`

-`export VK_LAYER_PATH=path/to/vulkansdk/macOS/share/vulkan/explicit_layer.d`

### [TODO: GLFW bug on Windows only, will silently fail to create a surface]

## Brief Info

This project uses CMake. If you are testing the editor, make sure to build and install the Starry sub-project first.

Currently, the Starry API is enclosed in the starry subproject that builds to a static library.
Starry outputs to `[STARRY_DIR]/bin/[SYSTEM_NAME]`. The static library builds to `/lib` and the included glfw subproject builds to a shared library under `/dyn`.

Starry's renderer currently supports window resizing, MSAA, Dear ImGui GUI integration, and is build on the Vulkan Graphics API and GLFW.

Starry also has an internal project called "manager" which oversees object resource sharing, file handling, logging, and fatal errors. If file-logging is set to true, Starry will output a log file to `starry_out/log`. Vulkan validation layers and the GLFW error callback also run through the logger.

Editor is a project that utilizes the Starry API to present a render. A soon to be editor for scenes. Editor outputs it's executable to `[EDITOR_DIR]/bin/[SYSTEM_NAME]/run`

### Example render w/ radio.obj (Test model)
Image:
![Example Render GIF](/docs/media/example1.png)

Gif:
![Example Render GIF](/docs/media/example_radio.gif)