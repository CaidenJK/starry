cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryEditor)

project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES C CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR " ${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

include (FindVulkan)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
else()
  find_package(Vulkan REQUIRED)
endif()

set(SHADER_DIR "${SOURCE_DIR}/shaders")
get_filename_component(SHADER_DIR_ABS "${SHADER_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${SHADER_DIR_ABS}" SHADER_DIR_UNIX)

set(VERT_SHADER "simple_shader.vert")
set(FRAG_SHADER "simple_shader.frag")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  execute_process(COMMAND cmd /C "${Vulkan_GLSLC_EXECUTABLE} ${SHADER_DIR}/${VERT_SHADER} -o ${SHADER_DIR}/vert.spv" )
  execute_process(COMMAND cmd /C "${Vulkan_GLSLC_EXECUTABLE} ${SHADER_DIR}/${FRAG_SHADER} -o ${SHADER_DIR}/frag.spv" )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  execute_process(COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${SHADER_DIR}/${VERT_SHADER}" -o "${SHADER_DIR}/vert.spv")
  execute_process(COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${SHADER_DIR}/${FRAG_SHADER}" -o "${SHADER_DIR}/frag.spv")
else()
  message(FATAL_ERROR "Unsupported OS")
endif()
message("Compiled Shaders")

# Image Directory
set(IMAGE_DIR "${SOURCE_DIR}/images")
get_filename_component(IMAGE_DIR_ABS "${IMAGE_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${IMAGE_DIR_ABS}" IMAGE_DIR_UNIX)

# Models Directory
set(MODEL_DIR "${SOURCE_DIR}/models")
get_filename_component(MODEL_DIR_ABS "${MODEL_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${MODEL_DIR_ABS}" MODEL_DIR_UNIX)

find_package(Starry)
include_directories(
    Starry
)

include_directories(
    ${INCLUDE_DIR}
)

# ------------------------------------
add_library(${PROJECT_NAME} ${SOURCES})
# ------------------------------------

target_compile_definitions(${PROJECT_NAME} PRIVATE "SHADERS_PATH=\"${SHADER_DIR_UNIX}/\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE "IMAGE_PATH=\"${IMAGE_DIR_UNIX}/\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE "MODEL_PATH=\"${MODEL_DIR_UNIX}/\"")

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "${PROJECT_NAME}"
)