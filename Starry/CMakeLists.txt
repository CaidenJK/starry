# Add project
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME Starry)

project(${PROJECT_NAME} VERSION 0.0.5 LANGUAGES C CXX)

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/bin)

# Set Sources

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

# Includes

# Vulkan
include (FindVulkan)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_package(Vulkan REQUIRED COMPONENTS MoltenVK)
  if (NOT Vulkan_MoltenVK_FOUND)
    message(FATAL_ERROR "Could not find MoltenVK for MacOS Dev")
  endif()
else()
  find_package(Vulkan REQUIRED)
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/StarryRender" "${CMAKE_CURRENT_BINARY_DIR}/StarryRender_build")
add_subdirectory("${CMAKE_SOURCE_DIR}/StarryManager" "${CMAKE_CURRENT_BINARY_DIR}/StarryManager_build")

find_package(StarryRender)
find_package(StarryManager)
include_directories(
    StarryRender
    StarryManager
)

include_directories(
    ${INCLUDE_DIR}
    ${glfw_SOURCE_DIR}/include
    ${glad_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/../StarryRender/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../StarryManager/include"
)

# -------------------------
add_library(${PROJECT_NAME} SHARED ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE STARRY_BUILD_DLL)
# -------------------------

set_target_properties(${PROJECT_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}/bin"
  ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}/lib"
  OUTPUT_NAME "${PROJECT_NAME}"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(${PROJECT_NAME} PROPERTIES
    FRAMEWORK FALSE
    MACOSX_RPATH ON
    CMAKE_CXX_VISIBILITY_PRESET hidden
    CMAKE_VISIBILITY_INLINES_HIDDEN ON
  )
endif()

# Link Libraries
target_link_libraries(Starry 
    PRIVATE 
    glm
    glfw
    Vulkan::Vulkan
    StarryManager
    StarryRender
)
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  find_library(IOSURFACE_FRAMEWORK IOSurface)
  target_link_libraries(Starry PRIVATE ${IOSURFACE_FRAMEWORK})
endif()