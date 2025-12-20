# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.


# Cmake Version
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

# Set Sources
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

set(SOURCES
    ${SOURCE_DIR}/main.cpp
)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Project Name and Language
project ("StarryApp" VERSION 0.0.1 LANGUAGES C CXX) 

add_subdirectory("Starry")
find_package(Starry)

# add executable
add_executable(StarryApp ${SOURCES})

# link Libraries
target_link_libraries(StarryApp PRIVATE Starry)

if (TARGET Starry)
  add_custom_command(TARGET StarryApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:Starry>"
      "$<TARGET_FILE_DIR:StarryApp>"
    COMMENT "Copying Starry Dynamic Library next to StarryApp executable"
    VERBATIM)
endif()