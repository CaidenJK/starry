cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryRender)

project(${PROJECT_NAME} VERSION 0.0.3 LANGUAGES C CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

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

add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

# --------------------------------------------------------------

# Vulkan
include (FindVulkan)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
else()
  find_package(Vulkan REQUIRED)
endif()

# IMGUI
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
FILE(GLOB IMGUI_SOURCES "${IMGUI_DIR}/*.cpp")

add_library(imgui_lib ${IMGUI_SOURCES} "${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp")

target_include_directories(imgui_lib
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}

    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# ------------------------------
add_library(${PROJECT_NAME} ${SOURCES})
# ------------------------------

target_include_directories( ${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_DIR}
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}

    ${CMAKE_SOURCE_DIR}/StarryManager

    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_include_directories( ${PROJECT_NAME}
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "${PROJECT_NAME}"
)