include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

function(add_comics_test name)
    add_executable(test-${name}-comics test-${name}.cpp)
    target_link_libraries(test-${name}-comics comics GTest::gmock_main)
    set_property(TARGET test-${name}-comics PROPERTY FOLDER "Tests")
    gtest_discover_tests(test-${name}-comics)
endfunction()

add_comics_test(cobalt)
add_comics_test(coro)
