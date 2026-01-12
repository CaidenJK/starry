# ------------------------------- Project Settings -------------------------------
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryEditor)

set(VERSION "0.0.1")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES C CXX)

set(EXE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/run")

# Set Sources
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXE_DIR})

# ------------------------------- Vulkan -------------------------------

# Vulkan
include (FindVulkan)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
  if (NOT Vulkan_MoltenVK_FOUND)
    message(FATAL_ERROR "Could not find MoltenVK for MacOS Dev")
  endif()
else()
  find_package(Vulkan REQUIRED)
endif()

set(SHADER_DIR "${SOURCE_DIR}/shaders")
get_filename_component(SHADER_DIR_ABS "${SHADER_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${SHADER_DIR_ABS}" SHADER_DIR_UNIX)

# ------------------------------- Shaders (for now) -------------------------------

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

# ------------------------------- Starry -------------------------------

set(Starry_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Starry)
find_package(Starry CONFIG REQUIRED)

# ------------------------------- EXE -------------------------------

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Vulkan::Vulkan
    Starry::starry
    Starry::glfw
)

# ------------------------------- Build Definitions -------------------------------

# Image Directory
set(IMAGE_DIR "${SOURCE_DIR}/images")
get_filename_component(IMAGE_DIR_ABS "${IMAGE_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${IMAGE_DIR_ABS}" IMAGE_DIR_UNIX)

# Models Directory
set(MODEL_DIR "${SOURCE_DIR}/models")
get_filename_component(MODEL_DIR_ABS "${MODEL_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${MODEL_DIR_ABS}" MODEL_DIR_UNIX)

target_compile_definitions(${PROJECT_NAME} PRIVATE "SHADERS_PATH=\"${SHADER_DIR_UNIX}/\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE "IMAGE_PATH=\"${IMAGE_DIR_UNIX}/\"")
target_compile_definitions(${PROJECT_NAME} PRIVATE "MODEL_PATH=\"${MODEL_DIR_UNIX}/\"")

# ------------------------------- Copy Resources -------------------------------

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
       add_custom_command(
        TARGET StarryEditor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/Starry/bin/Windows/dyn/glfw3.dll
                ${EXE_DIR}/glfw3.dll)
    endif()