
set(target lemon_adapter)

add_library(${target} SHARED include/arbok/others/lemon.h lemon.cpp)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

FIND_PATH(LEMON_SOURCE_ROOT_DIR CMakeLists.txt
        PATHS ${PROJECT_SOURCE_DIR}/lemon
        NO_DEFAULT_PATH
        DOC "Location of LEMON source as a CMAKE subproject")
if (EXISTS ${LEMON_SOURCE_ROOT_DIR})
    ADD_SUBDIRECTORY(${LEMON_SOURCE_ROOT_DIR} deps/lemon)
    target_link_libraries(${target} lemon)
    target_compile_definitions(${target} PRIVATE LEMON) # macro to disable lemon code if not found
    target_include_directories(${target} PRIVATE
            ${LEMON_SOURCE_ROOT_DIR}
            ${CMAKE_BINARY_DIR}/deps/lemon
    )
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 17) # lemon breaks in c++20
else ()
    message(STATUS "lemon not found. lemon algo will not be functional.")
endif()
