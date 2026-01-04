# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.

# Cmake Version
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME App)

project (${PROJECT_NAME} VERSION 0.0.1 LANGUAGES C CXX)

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

add_subdirectory("Starry")
find_package(Starry)

add_subdirectory("StarryEditor")
find_package(StarryEditor)

# add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# link Libraries
target_link_libraries(${PROJECT_NAME} PRIVATE StarryEditor)
target_link_libraries(${PROJECT_NAME} PRIVATE Starry)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
)

if (TARGET Starry)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:Starry>"
      "$<TARGET_FILE_DIR:App>"
    COMMENT "Copying Starry Dynamic Library next to App executable"
    VERBATIM)
endif()