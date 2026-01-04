cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryRender)

project(${PROJECT_NAME} VERSION 0.0.3 LANGUAGES C CXX)


set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# DEPENDENCIES
# ---------------------------------------

set(FETCHCONTENT_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies")
include(FetchContent)

# Don't build bloat
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
    SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/glfw"
)

# GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
    SOURCE_DIR "${FETCHCONTENT_BASE_DIR}/glm"
)


FetchContent_Populate(glfw)
FetchContent_Populate(glm)

FetchContent_MakeAvailable(glfw glm)

# Vulkan
include (FindVulkan)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
else()
  find_package(Vulkan REQUIRED)
endif()

find_package(StarryManager)
include_directories(
    StarryManager
)

include_directories(
    ${INCLUDE_DIR}
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

# ------------------------------
add_library(${PROJECT_NAME} ${SOURCES})
# ------------------------------

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "${PROJECT_NAME}"
)