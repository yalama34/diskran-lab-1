cmake_minimum_required(VERSION 3.16)
project(diskran_lab_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(b_tree src/b_tree.cpp src/serialization.cpp)
target_include_directories(b_tree PUBLIC src/include)
add_executable(dictionary src/main.cpp)
target_link_libraries(dictionary PRIVATE b_tree)

option(BUILD_TESTING "Build tests (requires Google Test)" OFF)
include(CTest)
if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.15.2
            GIT_SHALLOW TRUE
        )
        # Keep the same runtime library for Google Test and the project on MSVC.
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    include(GoogleTest)
    foreach(test_name IN ITEMS insert random persistence)
        add_executable(test_${test_name} tests/test_${test_name}.cpp)
        target_link_libraries(test_${test_name} PRIVATE b_tree GTest::gtest_main)
        gtest_discover_tests(test_${test_name})
    endforeach()

    find_package(Python3 COMPONENTS Interpreter)
    if(Python3_Interpreter_FOUND)
        add_test(NAME cli COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/test_cli.py $<TARGET_FILE:dictionary>)
    endif()
endif()
