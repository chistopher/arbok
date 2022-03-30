
set(target lemon_adapter)

add_library(${target} SHARED include/arbok/lemon/lemon.h lemon.cpp)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(${target} arbok lemon)
set_property(TARGET ${target} PROPERTY CXX_STANDARD 17) # lemon breaks in c++20
