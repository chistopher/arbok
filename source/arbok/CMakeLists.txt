
find_package(Boost)
find_package(OpenMP REQUIRED)

set(target arbok)

set(HEADERS
    include/${target}/gabow.h
)

set(SOURCES
    gabow.cpp
)

configure_file(include/${target}/paths.h.in include/${target}/paths.h)

add_library(${target} SHARED ${HEADERS} ${SOURCES})

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_BINARY_DIR}/include)

