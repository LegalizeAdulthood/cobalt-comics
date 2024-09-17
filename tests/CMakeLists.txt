include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-cobalt-comics test.cpp)
target_link_libraries(test-cobalt-comics GTest::gtest_main comics)

gtest_discover_tests(test-cobalt-comics)
