cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Project configuration
project(3DModelManagementUtility
    VERSION 1.0.0
    DESCRIPTION "A fluid, intuitive 3D model management system with CNC integration"
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find required packages
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Widgets
    OpenGL
    OpenGLWidgets
    Sql
    Concurrent
    Charts
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find additional libraries
if(WIN32)
    # On Windows, use the pre-built Assimp library
    set(ASSIMP_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/install/include)
    set(ASSIMP_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/install/lib/assimp-vc143-mt.lib)
    set(ASSIMP_FOUND TRUE)
else()
    # On Unix systems, use PkgConfig
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ASSIMP REQUIRED assimp)
endif()

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Create executable
qt6_standard_project_setup()

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CORE_DIR ${SRC_DIR}/core)
set(UI_DIR ${SRC_DIR}/ui)
set(RENDER_DIR ${SRC_DIR}/render)
set(REPAIR_DIR ${SRC_DIR}/repair)
set(NESTING_DIR ${SRC_DIR}/nesting)
set(TOOLS_DIR ${SRC_DIR}/tools)
set(SIMULATION_DIR ${SRC_DIR}/simulation)
set(UTILS_DIR ${SRC_DIR}/utils)

# Include directories
include_directories(
    ${SRC_DIR}
    ${CORE_DIR}
    ${UI_DIR}
    ${RENDER_DIR}
    ${REPAIR_DIR}
    ${NESTING_DIR}
    ${TOOLS_DIR}
    ${SIMULATION_DIR}
    ${UTILS_DIR}
)

if(WIN32)
    include_directories(${ASSIMP_INCLUDE_DIRS})
endif()

# Source files
file(GLOB_RECURSE SOURCES
    "${CORE_DIR}/*.cpp"
    "${UI_DIR}/*.cpp"
    "${RENDER_DIR}/*.cpp"
    "${REPAIR_DIR}/*.cpp"
    "${NESTING_DIR}/*.cpp"
    "${TOOLS_DIR}/*.cpp"
    "${SIMULATION_DIR}/*.cpp"
    "${UTILS_DIR}/*.cpp"
)

# Header files for MOC
file(GLOB_RECURSE HEADERS
    "${CORE_DIR}/*.h"
    "${UI_DIR}/*.h"
    "${RENDER_DIR}/*.h"
    "${REPAIR_DIR}/*.h"
    "${NESTING_DIR}/*.h"
    "${TOOLS_DIR}/*.h"
    "${SIMULATION_DIR}/*.h"
    "${UTILS_DIR}/*.h"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    Qt6::Sql
    Qt6::Concurrent
    Qt6::Charts
    OpenGL::GL
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES})
endif()

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive- /Zc:__cplusplus>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)

# Define version macros
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_VERSION="${PROJECT_VERSION}"
    PROJECT_NAME="${PROJECT_NAME}"
)

# Copy Qt runtime libraries on Windows
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON
        QT_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Print configuration summary
message(STATUS "=== ${PROJECT_NAME} Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Qt6 version: ${Qt6_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=====================================")