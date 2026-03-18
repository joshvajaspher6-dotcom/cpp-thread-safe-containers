cmake_minimum_required(VERSION 3.14)
project(ThreadSafeContainers)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include headers
include_directories(include)

# main demo executable
add_executable(demo src/main.cpp)
target_link_libraries(demo pthread)

# Google Test setup
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
     DOWNLOAD_EXTRACT_TIMESTAMP true 
)
FetchContent_MakeAvailable(googletest)

# test executables
add_executable(test_queue   tests/test_queue.cpp)
add_executable(test_stack   tests/test_stack.cpp)
add_executable(test_vector  tests/test_vector.cpp)
add_executable(test_list    tests/test_list.cpp)
add_executable(test_map     tests/test_map.cpp)

# link each test with gtest and pthread
foreach(test_target
    test_queue
    test_stack
    test_vector
    test_list
    test_map)
    target_link_libraries(${test_target}
        gtest_main
        pthread
    )
endforeach()

# enable testing
enable_testing()
add_test(NAME QueueTest  COMMAND test_queue)
add_test(NAME StackTest  COMMAND test_stack)
add_test(NAME VectorTest COMMAND test_vector)
add_test(NAME ListTest   COMMAND test_list)
add_test(NAME MapTest    COMMAND test_map)