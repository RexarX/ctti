include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install the header-only library
install(TARGETS ctti
    EXPORT ctti_targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Create version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

# Create and install config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctti_config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

# Install CMake config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

# Install targets file
install(EXPORT ctti_targets
    FILE ctti-targets.cmake
    NAMESPACE ctti::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

# Export for build tree
export(EXPORT ctti_targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ctti-targets.cmake"
    NAMESPACE ctti::
)

export(PACKAGE ctti)
