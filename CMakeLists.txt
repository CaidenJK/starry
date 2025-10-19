# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Starry" VERSION 0.0.1 LANGUAGES C CXX)

# Include FetchContent for dependencies
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

#GLAD
FetchContent_Declare(
    glad 
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.4
    GIT_SHALLOW true
)

#GLM
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)

FetchContent_Populate(glfw)
FetchContent_Populate(glad)
FetchContent_Populate(glm)

# Include sub-projects.
add_subdirectory ("StarryApp")

add_executable(Starry "src/main.cpp")
target_link_libraries(Starry StarryApp)
