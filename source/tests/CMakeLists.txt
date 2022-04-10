
find_package(GTest)

if (NOT GTEST_FOUND)
    return()
endif (NOT GTEST_FOUND)

set(target fulltests)

add_executable(${target}
    test_find_data.cpp
    test_compressed_tree.cpp
    test_fastestspeedrun.cpp
)

target_link_libraries(${target} GTest::Main arbok atofigh_adapter lemon_adapter)
gtest_discover_tests(${target})

