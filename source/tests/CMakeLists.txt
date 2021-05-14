
find_package(GTest)

if (NOT GTEST_FOUND)
    return()
endif (NOT GTEST_FOUND)

set(target tests)

add_executable(${target}
    test_find_data.cpp
    test_fastestspeedrun.cpp
)

target_link_libraries(${target} GTest::Main arbok)

gtest_discover_tests(${target})

