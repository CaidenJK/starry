# Add project
cmake_minimum_required (VERSION 3.10)
project(StarryAsset VERSION 0.0.1 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Set Headers

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starryasset")

# Create Library
add_library(StarryAsset ${SOURCES})

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set_target_properties(StarryAsset PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "StarryAsset"
)

# Gather Dependency Include Directories
target_include_directories(StarryAsset
  PUBLIC
    ${INCLUDE_DIR}
)