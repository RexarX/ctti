cmake_minimum_required(VERSION 3.24)

project(ctti
    VERSION 1.1.0
    DESCRIPTION "Compile Time Type Introspection library"
    HOMEPAGE_URL "https://github.com/RexarX/ctti"
    LANGUAGES CXX
)

# Define CTTI_ROOT_DIR
set(CTTI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "CTTI root directory")

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Enable IDE support (Visual Studio, Xcode, etc.)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Set output directories for binaries
    set(_output_dir "${CTTI_ROOT_DIR}/bin/$<LOWER_CASE:${CMAKE_SYSTEM_NAME}>-$<LOWER_CASE:${CMAKE_SYSTEM_PROCESSOR}>")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${_output_dir})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${_output_dir})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${_output_dir})
endif()

option(CTTI_BUILD_TESTS "Build CTTI tests" ${PROJECT_IS_TOP_LEVEL})
option(CTTI_BUILD_EXAMPLES "Build CTTI examples" ${PROJECT_IS_TOP_LEVEL})
option(CTTI_INSTALL "Install CTTI" ${PROJECT_IS_TOP_LEVEL})

add_library(ctti INTERFACE)
add_library(ctti::ctti ALIAS ctti)

target_include_directories(ctti SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ctti INTERFACE cxx_std_20)

if(CTTI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CTTI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CTTI_INSTALL)
    include(cmake/install.cmake)
endif()
