# ------------------------------- Project Settings -------------------------------
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME StarryBuild)

set(VERSION "0.1.0")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES C CXX)

# ------------------------------- Options -------------------------------

option(BUILD_EDITOR "Build the Editor executable" ON)
option(BUILD_STARRY_LIB "Build the Starry library only" OFF)

# ------------------------------- Subdirectories -------------------------------

if(BUILD_STARRY_LIB OR BUILD_EDITOR)
    add_subdirectory(Starry ${CMAKE_SOURCE_DIR}/Starry/build)
endif()

if(BUILD_EDITOR)
   add_subdirectory(Editor ${CMAKE_SOURCE_DIR}/Editor/build)
endif()
