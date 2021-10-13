
add_executable(sandbox sandbox.cpp)
target_link_libraries(sandbox arbok)

add_executable(compprog1 compprog1.cpp)
target_link_libraries(compprog1 arbok)

add_executable(compprog2 compprog2.cpp)
target_link_libraries(compprog2 arbok)

add_executable(fastestspeedrun fastestspeedrun.cpp)
target_link_libraries(fastestspeedrun arbok)

add_executable(fastestspeedrun2 fastestspeedrun2.cpp)

add_executable(gabow_test gabow_test.cpp)
target_link_libraries(gabow_test arbok)