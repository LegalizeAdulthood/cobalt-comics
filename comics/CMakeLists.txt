find_package(simdjson CONFIG REQUIRED)

find_package(boost_cobalt REQUIRED CONFIG)

add_library(comics
    include/comics/cobalt.h
    include/comics/comics.h
    include/comics/coro.h
    include/comics/database.h
    ends_with.h
    cobalt.cpp
    comics.cpp
    coro.cpp
    json_database.cpp
)
target_include_directories(comics PUBLIC include)
target_include_directories(comics PRIVATE .)
target_link_libraries(comics PUBLIC simdjson::simdjson Boost::cobalt)
set_target_properties(comics PROPERTIES FOLDER "Libraries")
