include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ctti
    EXPORT ctti-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT ctti-targets
    FILE ctti-targets.cmake
    NAMESPACE ctti::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctti-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ctti-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ctti
)

export(EXPORT ctti-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/ctti-targets.cmake"
    NAMESPACE ctti::
)

export(PACKAGE ctti)
