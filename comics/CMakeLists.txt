find_package(simdjson CONFIG REQUIRED)

find_package(boost_cobalt REQUIRED CONFIG)

add_library(comics
    include/comics/comics.h
    include/comics/coro.h
    comics.cpp
    coro.cpp
)
target_include_directories(comics PUBLIC include)
target_link_libraries(comics PUBLIC simdjson::simdjson Boost::cobalt)
set_target_properties(comics PROPERTIES FOLDER "Libraries")
