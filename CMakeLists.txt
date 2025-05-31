cmake_minimum_required(VERSION 3.24)
project(ctti VERSION 0.1.0 LANGUAGES CXX)

option(CTTI_BUILD_TESTS "Build ctti feature tests" OFF)
option(CTTI_BUILD_EXAMPLES "Build ctti examples" OFF)

add_library(ctti INTERFACE)
target_compile_features(ctti INTERFACE cxx_std_23)
target_include_directories(ctti INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(ctti INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -pedantic>)
elseif(MSVC)
    target_compile_options(ctti INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:/W4>)
endif()

include(cmake/vs_source_groups.cmake)

include(GNUInstallDirs)
install(TARGETS ctti
    EXPORT ctti-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ctti-targets
    FILE ctti-targets.cmake
    NAMESPACE ctti::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctti-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

if(CTTI_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(CTTI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
