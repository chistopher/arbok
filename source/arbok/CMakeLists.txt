
find_package(Boost)
find_package(OpenMP REQUIRED)

set(target arbok)

set(HEADERS
        include/${target}/dsu.h
        include/${target}/gabow.h
        include/${target}/tarjan.h
        include/${target}/treap.h

)

set(SOURCES
        dsu.cpp
        gabow.cpp
        tarjan.cpp
        tarjan_impl.h
        tarjan_matrix.cpp
        tarjan_set.cpp
        treap.cpp
)

configure_file(include/${target}/paths.h.in include/${target}/paths.h)

add_library(${target} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

