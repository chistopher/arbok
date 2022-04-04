
find_package(OpenMP REQUIRED)

set(target arbok)

set(HEADERS
        include/${target}/data_structures/dsu.h
        include/${target}/gabow/gabow.h
        include/${target}/gabow/active_forest.h
        include/${target}/tarjan/tarjan.h
        include/${target}/tarjan/impl.h
        include/${target}/data_structures/treap.h
        include/${target}/data_structures/compressed_tree.h
        include/${target}/data_structures/compressed_tree.inl
        include/${target}/data_structures/hollow_heap.h
        include/${target}/utils/cf_solution.h
        include/${target}/others/felerius.h
        include/${target}/others/spaghetti.h
)

set(SOURCES
        src/utils/cf_solution.cpp
        src/data_structures/dsu.cpp
        src/data_structures/treap.cpp
        src/gabow/gabow.cpp
        src/gabow/active_forest.cpp
        src/tarjan/tarjan.cpp
        src/tarjan/tarjan_matrix.cpp
        src/tarjan/tarjan_set.cpp
        src/tarjan/tarjan_pq.cpp
        src/tarjan/tarjan_hh.cpp
        src/tarjan/tarjan_treap.cpp
)

configure_file(include/${target}/utils/paths.h.in include/${target}/utils/paths.h)

add_library(${target} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

