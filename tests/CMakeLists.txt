add_executable(ctti_tests
    main.cpp

    test_type_tag.cpp
    test_static_value.cpp
    test_concepts.cpp
    test_name.cpp
    test_symbol.cpp
    test_attributes.cpp
    test_constructor.cpp
    test_enum_utils.cpp
    test_hash.cpp
    test_inheritance.cpp
    test_map.cpp
    test_model.cpp
    test_template_info.cpp
    test_tie.cpp
    test_type_id.cpp
    test_reflection.cpp

    detail/test_meta.cpp
    detail/test_pretty_function.cpp
    detail/test_name_filters.cpp
    detail/test_name_parser.cpp
    detail/test_entity_name.cpp
    detail/test_overload_resolution.cpp
    detail/test_compile_time_string.cpp
)

target_link_libraries(ctti_tests PRIVATE ctti)
target_include_directories(ctti_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(ctti_tests PROPERTIES
    CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF
)

target_compile_features(ctti_tests PRIVATE cxx_std_23)

add_test(NAME ctti_tests COMMAND ctti_tests)

set_tests_properties(ctti_tests PROPERTIES
    ENVIRONMENT "DOCTEST_OPTIONS=--no-breaks"
)
