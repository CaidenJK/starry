# Add project
project(StarryRender VERSION 0.0.3 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Set Headers

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starryrender")

# Vulkan

include (FindVulkan)
find_package(Vulkan REQUIRED)

# Create Library
add_library(StarryRender ${SOURCES})

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set_target_properties(StarryRender PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "StarryRender"
)

set(STARRY_LOG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/../StarryLog/include")

# Gather Dependency Include Directories
target_include_directories(StarryRender
  PUBLIC
    ${INCLUDE_DIR}
    ${STARRY_LOG_INCLUDE}
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)