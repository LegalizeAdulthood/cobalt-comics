include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-comics
    parsed_json.h
    test-cobalt.cpp
    test-coro.cpp
)
target_link_libraries(test-comics comics GTest::gmock_main)
set_property(TARGET test-comics PROPERTY FOLDER "Tests")
gtest_discover_tests(test-comics)
