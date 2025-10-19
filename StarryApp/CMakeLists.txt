# CMakeList.txt : CMake project for StarryApp, include source and define
# project specific logic here.
#

# Include Header Dir
include_directories(src/headers)

# Set Sources
set(SOURCES src/Application.cpp)

# Create Library
add_library (StarryApp ${SOURCES})

# C++ 20
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StarryApp PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.