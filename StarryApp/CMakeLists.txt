# CMakeList.txt : CMake project for StarryApp, include source and define
# project specific logic here.

# Add project
project(StarryApp VERSION 0.0.1 LANGUAGES C CXX)

# Set Sources
set(SOURCES src/Application.cpp)

# Set Headers
set(HEADERS 
	src/headers/Application.h
	src/headers/Window.h
	src/headers/Renderer.h
	src/headers/Scene.h
)

# Create Library
add_library(StarryApp ${SOURCES} ${HEADERS})

# Gather Dependency Include Directories
target_include_directories(StarryApp
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
)

# C++ 20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StarryApp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests