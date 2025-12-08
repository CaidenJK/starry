# CMakeList.txt : CMake project for StarryApp, include source and define
# project specific logic here.

# Add project
project(StarryRender VERSION 0.0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Set Headers

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starryrender")

set(VULKAN_DIR 
	"C:/VulkanSDK/1.4.313.2"
)

# Create Library
add_library(StarryRender ${SOURCES})

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set_target_properties(StarryRender PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "StarryRender"
)

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