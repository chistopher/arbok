
add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox arbok lemon_adapter)

add_executable(arbok-cli arbok-cli.cpp)
target_link_libraries(arbok-cli arbok lemon_adapter)

