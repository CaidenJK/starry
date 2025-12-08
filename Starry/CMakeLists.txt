# CMakeList.txt : CMake project for StarryApp, include source and define
# project specific logic here.

# Add project
project(Starry VERSION 0.0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/bin)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Set Headers

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starry")

set(VULKAN_DIR 
        "C:/VulkanSDK/1.4.313.2"
)

# Compile Shaders

set(SHADER_DIR "${SOURCE_DIR}/shaders")

get_filename_component(SHADER_DIR_ABS "${SHADER_DIR}" ABSOLUTE)
file(TO_CMAKE_PATH "${SHADER_DIR_ABS}" SHADER_DIR_UNIX)

set(VERT_SHADER "simple_shader.vert")
set(FRAG_SHADER "simple_shader.frag")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    execute_process(COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe ${SHADER_DIR}/${VERT_SHADER} -o ${SHADER_DIR}/vert.spv" )
    execute_process(COMMAND cmd /C "${VULKAN_DIR}/Bin/glslc.exe ${SHADER_DIR}/${FRAG_SHADER} -o ${SHADER_DIR}/frag.spv" )
    message("Compiled Shaders for Windows")
else()
     message(FATAL_ERROR "Unsupported OS")  
endif()

# DEPENDENCIES
# ---------------------------------------

# Set directory name for FetchContent
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/Dependencies")

# Include FetchContent for dependencies
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

# Populate FetchContent
FetchContent_Populate(glfw)
FetchContent_Populate(glm)

# Make Available
FetchContent_MakeAvailable(glfw glm)

# PROJECT BUILD
# ---------------------------------------

# Include Libraries to Cmake Build System such that CMake builds them
add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../StarryRender" "${CMAKE_CURRENT_BINARY_DIR}/StarryRender_build")

# Create Library
add_library(Starry SHARED ${SOURCES})
target_compile_definitions(Starry PRIVATE STARRY_BUILD_DLL)
target_compile_definitions(Starry PRIVATE "SHADERS_PATH=\"${SHADER_DIR_UNIX}/\"")

set_target_properties(Starry PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "Starry"
)

# Link Libraries
target_link_libraries(Starry PRIVATE glm)
target_link_libraries(Starry PRIVATE glfw)
target_link_libraries(Starry PRIVATE ${VULKAN_DIR}/Lib/vulkan-1.lib)

target_link_libraries(Starry PRIVATE StarryRender)

# Gather Dependency Include Directories
target_include_directories(Starry
  PUBLIC
    ${INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
	${VULKAN_DIR}/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../StarryRender/include
)

# TODO: Add tests
# PROBLEM: Code not portable because of vulkan sdk path