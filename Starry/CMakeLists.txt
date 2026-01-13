# ------------------------------- Project Settings -------------------------------
cmake_minimum_required (VERSION 3.10)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Debug) # Symbols

set(PROJECT_NAME Starry)

set(VERSION "0.0.6")

project(${PROJECT_NAME} VERSION ${VERSION} LANGUAGES C CXX)

set(LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}")
set(INTERMEDITE_DIR "int")
set(STATIC_DIR "${LIBRARY_DIR}/lib")
set(SHARED_DIR "${LIBRARY_DIR}/dyn")
set(EXPORT_DIR "${LIBRARY_DIR}/export")
set(INSTALL_DIR "${LIBRARY_DIR}/install")

# Set Sources
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/starry")

FILE(GLOB SOURCES "${SOURCE_DIR}/*.cpp")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${STATIC_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SHARED_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SHARED_DIR})

# ------------------------------- Guards -------------------------------

if ((NOT (( CMAKE_SYSTEM_NAME STREQUAL "Darwin") OR (CMAKE_SYSTEM_NAME STREQUAL "Windows"))) OR (NOT CMAKE_SIZEOF_VOID_P EQUAL 8))
  message(FATAL_ERROR "Starry only supports 64-bit MacOS and Windows systems!")
endif()

# ------------------------------- Vulkan -------------------------------

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

# ------------------------------- Subdirectories -------------------------------

add_subdirectory(s_manager ${INTERMEDITE_DIR}/s_manager)
add_subdirectory(s_renderer ${INTERMEDITE_DIR}/s_renderer)

# ------------------------------- GLFW Shared Lib -------------------------------
set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(GLFW_LIBRARY_TYPE SHARED)
set(GLFW_DIR "s_renderer/external/glfw")

add_subdirectory(${GLFW_DIR} "${INTERMEDITE_DIR}/glfw")

set_target_properties(glfw PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${SHARED_DIR}
  LIBRARY_OUTPUT_DIRECTORY ${SHARED_DIR}
  ARCHIVE_OUTPUT_DIRECTORY ${STATIC_DIR}
)

# ------------------------------- Lib -------------------------------
set (MAIN_LIB starry)
add_library(${MAIN_LIB} STATIC 
  ${SOURCES}
  $<TARGET_OBJECTS:s_manager>
  $<TARGET_OBJECTS:s_renderer> 
  $<TARGET_OBJECTS:imgui_lib> 
)
target_compile_definitions(${MAIN_LIB} PRIVATE STARRY_BUILD)
target_compile_definitions(${MAIN_LIB} PRIVATE "VERSION=\"${VERSION}\"")


if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set_target_properties(${MAIN_LIB} PROPERTIES
    FRAMEWORK FALSE
    MACOSX_RPATH ON
    CMAKE_CXX_VISIBILITY_PRESET hidden
    CMAKE_VISIBILITY_INLINES_HIDDEN ON
  )
endif()

set_target_properties(${MAIN_LIB} PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${STATIC_DIR}"
  OUTPUT_NAME "${MAIN_LIB}"
)

target_link_libraries(${MAIN_LIB}
  PUBLIC
    s_manager
    s_renderer
)

target_include_directories(${MAIN_LIB}
  PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/s_renderer>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/s_manager>

    $<INSTALL_INTERFACE:../../../include>
    $<INSTALL_INTERFACE:../../../s_renderer/include>
    $<INSTALL_INTERFACE:../../../s_renderer/external/glfw/include>
    $<INSTALL_INTERFACE:../../../s_renderer/external/glm>
    $<INSTALL_INTERFACE:../../../s_renderer/external/imgui>
    $<INSTALL_INTERFACE:../../../s_renderer/external/imgui/backends>
    $<INSTALL_INTERFACE:../../../s_manager/include>
)

# ------------------------------- Install/Export Target -------------------------------

set(STARRY_TARGETS starryTargets)

install(TARGETS ${MAIN_LIB} s_manager s_renderer glfw
  EXPORT ${STARRY_TARGETS}
  ARCHIVE DESTINATION ${INSTALL_DIR}
)

install(EXPORT ${STARRY_TARGETS}
  NAMESPACE Starry::
  DESTINATION ${EXPORT_DIR}
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/StarryConfig.cmake"
        DESTINATION ${EXPORT_DIR})

# GLFW nonsense
install(DIRECTORY ${GLFW_DIR}/include/GLFW
        DESTINATION ${INSTALL_DIR}/include
        FILES_MATCHING PATTERN "*.h")