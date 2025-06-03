cmake_minimum_required(VERSION 3.24)

project(ctti
    VERSION 1.0.0
    DESCRIPTION "Compile Time Type Introspection library"
    HOMEPAGE_URL "https://github.com/RexarX/ctti"
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(CTTI_BUILD_TESTS "Build CTTI tests" ${PROJECT_IS_TOP_LEVEL})
option(CTTI_BUILD_EXAMPLES "Build CTTI examples" ${PROJECT_IS_TOP_LEVEL})
option(CTTI_INSTALL "Install CTTI" ${PROJECT_IS_TOP_LEVEL})
option(CTTI_USE_CPM "Use CPM for dependencies" ${PROJECT_IS_TOP_LEVEL})

add_library(ctti INTERFACE)
add_library(ctti::ctti ALIAS ctti)

target_include_directories(ctti INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(ctti INTERFACE cxx_std_23)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(ctti INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra>
    )
elseif(MSVC)
    target_compile_options(ctti INTERFACE
        $<$<COMPILE_LANGUAGE:CXX>:/permissive->
    )
endif()

if(CTTI_USE_CPM)
    include(cmake/CPM.cmake)
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    include(cmake/vs_source_groups.cmake)
    generate_vs_source_groups("Header Files" "${CMAKE_CURRENT_SOURCE_DIR}/include" CTTI_HEADERS)
    target_sources(ctti INTERFACE ${CTTI_HEADERS})
endif()

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
