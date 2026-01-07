cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryRender)
set(VERSION "0.0.6")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES C CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

# Dependencies ----------------------------------

set(DEPEND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# Don't build bloat
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_INSTALL OFF)

add_subdirectory("${DEPEND_DIR}/glm" "${BUILD_DIR}/glm")
add_subdirectory("${DEPEND_DIR}/glfw" "${BUILD_DIR}/glfw")

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

add_library(imgui_lib ${IMGUI_SOURCES} "${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp" "${IMGUI_DIR}/backends/imgui_impl_glfw.cpp")

target_include_directories(imgui_lib
    PRIVATE
    ${Vulkan_INCLUDE_DIRS}

    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends

    "${DEPEND_DIR}/glfw/include"
)

# ------------------------------
add_library(${PROJECT_NAME} ${SOURCES})
# ------------------------------

target_compile_definitions(${PROJECT_NAME} PRIVATE "VERSION=\"${VERSION}\"")

target_include_directories( ${PROJECT_NAME}
    PUBLIC
    ${INCLUDE_DIR}
    "${DEPEND_DIR}/glm"
    "${DEPEND_DIR}/glfw/include"
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