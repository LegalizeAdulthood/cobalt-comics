function(add_tool name)
    add_executable(comics-${name} ${name}.cpp)
    target_link_libraries(comics-${name} PUBLIC comics)
    set_property(TARGET comics-${name} PROPERTY FOLDER "Tools")
endfunction()

add_tool(synchronous)
add_tool(coroutine)
add_tool(cobalt)

add_executable(gcd-to-json gcd-to-json.cpp)
set_target_properties(gcd-to-json PROPERTIES FOLDER "Tools")
