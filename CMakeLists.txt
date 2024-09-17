cmake_minimum_required(VERSION 3.23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COBALT_COMICS_PROJECT_NAME "Cobalt Comics" CACHE "STRING" "Default project name for IDEs")
project("${COBALT_COMICS_PROJECT_NAME}" CXX)

include(CTest)

add_subdirectory(comics)
add_subdirectory(tools)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
