add_executable(ctti-example main.cpp)

set_target_properties(ctti-example PROPERTIES
    CXX_STANDARD 23
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
)

target_compile_features(ctti-example PRIVATE cxx_std_23)

target_link_libraries(ctti-example PRIVATE ctti)
