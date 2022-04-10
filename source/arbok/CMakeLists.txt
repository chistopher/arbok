
find_package(OpenMP REQUIRED)

set(target arbok)

set(HEADERS
        include/${target}/data_structures/dsu.h
        include/${target}/data_structures/compressed_tree.h
        include/${target}/data_structures/compressed_tree.inl
        include/${target}/data_structures/treap.h
        include/${target}/data_structures/hollow_heap.h
        include/${target}/tarjan/tarjan_base.h
        include/${target}/tarjan/tarjan_set.h
        include/${target}/tarjan/tarjan_pq.h
        include/${target}/tarjan/tarjan_matrix.h
        include/${target}/tarjan/tarjan_treap.h
        include/${target}/tarjan/tarjan_hollow.h
        include/${target}/gabow/gabow.h
        include/${target}/gabow/active_forest.h
        include/${target}/others/felerius.h
        include/${target}/others/spaghetti.h
        include/${target}/others/yosupo.h
        include/${target}/utils/utils.h
)

set(SOURCES
        src/data_structures/dsu.cpp
        src/data_structures/treap.cpp
        src/gabow/gabow.cpp
        src/gabow/active_forest.cpp
        src/utils/utils.cpp
)

configure_file(include/${target}/utils/paths.h.in include/${target}/utils/paths.h)

add_library(${target} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

