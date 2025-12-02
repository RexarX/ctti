add_executable(ctti_example main.cpp)

set_target_properties(ctti_example PROPERTIES
    CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
    FOLDER "Examples"
    RUNTIME_OUTPUT_DIRECTORY "${CTTI_ROOT_DIR}/bin/$<LOWER_CASE:${CMAKE_SYSTEM_NAME}>-$<LOWER_CASE:${CMAKE_SYSTEM_PROCESSOR}>"
)

target_compile_features(ctti_example PRIVATE cxx_std_20)

target_link_libraries(ctti_example PRIVATE ctti::ctti)
