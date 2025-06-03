cmake_minimum_required(VERSION 3.24)
project(test_ctti CXX)

find_package(ctti REQUIRED)

add_executable(test_ctti test_ctti.cpp)
target_link_libraries(test_ctti ctti::ctti)
target_compile_features(test_ctti PRIVATE cxx_std_23)

enable_testing()
add_test(NAME test_ctti COMMAND test_ctti)
