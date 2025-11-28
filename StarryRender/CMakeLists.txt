# CMakeList.txt : CMake project for StarryApp, include source and define
# project specific logic here.

# Add project
project(StarryRender VERSION 0.0.1 LANGUAGES C CXX)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(SOURCES 
    "${SOURCE_DIR}/Window.cpp"
    "${SOURCE_DIR}/Application.cpp"
    "${SOURCE_DIR}/RenderDevice.cpp"
    "${SOURCE_DIR}/RenderPipeline.cpp"
    "${SOURCE_DIR}/SwapChain.cpp"
)

# Set Headers

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starryrender")
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

set(VULKAN_DIR 
	"C:/VulkanSDK/1.4.313.2"
)

execute_process(COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe ${SHADER_DIR}/simple_shader.vert -o ${SHADER_DIR}/vert.spv" )
execute_process(COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe ${SHADER_DIR}/simple_shader.frag -o ${SHADER_DIR}/frag.spv" )

# Create Library
add_library(StarryRender ${SOURCES})

# Gather Dependency Include Directories
target_include_directories(StarryRender
  PUBLIC
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
	${VULKAN_DIR}/Include
)

# C++ 20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StarryRender PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests
# PROBLEM: Code not portable because of vulkan sdk path