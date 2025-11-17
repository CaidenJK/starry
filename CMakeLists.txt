# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

# Cmake Version
cmake_minimum_required (VERSION 3.8)

# Set directory name for FetchContent
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/Dependencies")

# Vulkan
set(VULKAN_DIR "C:/VulkanSDK/1.4.313.2")

# Set Sources

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

set(SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/StarryApplication.cpp
)

# Set Headers

set(HEADERS
    ${SOURCE_DIR}/StarryApplication.h
)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project Name and Language
project ("Starry" VERSION 0.0.1 LANGUAGES C CXX)

# DEPENDENCIES
# ---------------------------------------

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
add_subdirectory ("StarryRender")
add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})

# Add executable
add_executable(Starry ${SOURCES} ${HEADERS})

# Link Libraries
target_link_libraries(Starry glm)
target_link_libraries(Starry glfw)
target_link_libraries(Starry ${VULKAN_DIR}/Lib/vulkan-1.lib)
target_link_libraries(Starry StarryRender)

# C++ 20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Starry PROPERTY CXX_STANDARD 20)
endif()