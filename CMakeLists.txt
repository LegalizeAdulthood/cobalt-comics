cmake_minimum_required(VERSION 3.23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COBALT_COMICS_PROJECT_NAME "Cobalt Comics" CACHE "STRING" "Default project name for IDEs")
project("${COBALT_COMICS_PROJECT_NAME}" CXX)

include(CTest)

add_executable(query-comics main.cpp)

add_subdirectory(comics)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
