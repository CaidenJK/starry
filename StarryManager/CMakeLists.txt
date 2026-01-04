cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryManager)

project(${PROJECT_NAME} VERSION 0.0.1 LANGUAGES C CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/external")

include_directories(
    ${INCLUDE_DIR}
    ${EXTERN_DIR}
)

# ---------------------------------------
add_library(${PROJECT_NAME} ${SOURCES})
# ---------------------------------------

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "${PROJECT_NAME}"
)