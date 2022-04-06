
set(target atofigh_adapter)

add_library(${target} SHARED include/arbok/others/atofigh.h atofigh.cpp edmonds_optimum_branching_impl.hpp)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

FIND_PACKAGE(Boost)
if (${Boost_FOUND})
    target_include_directories(${target} PRIVATE ${Boost_INCLUDE_DIR})
    target_compile_definitions(${target} PRIVATE ATOFIGH) # macro to disable code if boost not found
else ()
    message(STATUS "boost not found. atofigh algo will not be functional.")
endif()
