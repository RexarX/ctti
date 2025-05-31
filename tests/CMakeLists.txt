add_subdirectory(static)

add_executable(ctti-test
    main.cpp
    hash.cpp
    nameof.cpp
    name_filters.cpp
    detailed_nameof.cpp
    tie.cpp
    map.cpp
    model.cpp
    symbol.cpp
    type_id_tests.cpp
    hash_literal_tests.cpp
    complex_model_tests.cpp
    metaprogramming_tests.cpp
    entity_name_tests.cpp
    advanced_symbol_tests.cpp
)

target_link_libraries(ctti-test PRIVATE ctti)

if(NOT MSVC)
    target_compile_options(ctti-test PRIVATE -ftemplate-backtrace-limit=0 -DCATCH_CONFIG_NO_POSIX_SIGNALS)
endif()

add_test(ctti-test ctti-test)
