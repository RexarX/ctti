add_library(ctti-static-tests
    hash.cpp
    symbol.cpp
    name.cpp
    static_value.cpp
    meta_tests.cpp
)

target_link_libraries(ctti-static-tests PRIVATE ctti)
