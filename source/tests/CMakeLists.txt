
find_package(GTest)

if (NOT GTEST_FOUND)
    return()
endif (NOT GTEST_FOUND)

set(target lighttests)

add_executable(${target}
    test_find_data.cpp
    test_compressed_tree.cpp
)

target_link_libraries(${target} GTest::Main arbok)
target_compile_options(${target} PRIVATE  -fsanitize=address -g)
target_link_options(${target} PRIVATE  -fsanitize=address)
gtest_discover_tests(${target})

set(target fulltests)

add_executable(${target}
    test_find_data.cpp
    test_compressed_tree.cpp
    test_fastestspeedrun.cpp
)

target_link_libraries(${target} GTest::Main arbok)
target_compile_options(${target} PRIVATE -fsanitize=address -g)
target_link_options(${target} PRIVATE -fsanitize=address)
gtest_discover_tests(${target})

