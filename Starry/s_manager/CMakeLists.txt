# ------------------------------- Project Settings -------------------------------
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME s_manager)

set(VERSION "0.0.6")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES C CXX)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/manager")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(DEPEND_DIR "${INCLUDE_DIR}/external")

# ------------------------------- Main Lib -------------------------------
set (MAIN_LIB s_manager)

add_library(${MAIN_LIB} OBJECT ${SOURCES})

target_compile_definitions(${MAIN_LIB} PRIVATE "VERSION=\"${VERSION}\"")

target_include_directories( ${MAIN_LIB}
    PRIVATE
    ${INCLUDE_DIR}
    ${EXTERN_DIR}
)

target_include_directories( ${MAIN_LIB}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

set_target_properties(${MAIN_LIB} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${MAIN_LIB}"
  LIBRARY_OUTPUT_DIRECTORY "${MAIN_LIB}"
  OUTPUT_NAME "${MAIN_LIB}"
)