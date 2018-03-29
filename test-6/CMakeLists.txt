cmake_minimum_required (VERSION 3.9.1)
project (test-6)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(test-6 main.cpp)

set_source_files_properties(test_adjacencymatrix.cpp        PROPERTIES COMPILE_FLAGS "-DR5_GTEST")
set_source_files_properties(test_adjacencymatrixindexer.cpp PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
target_link_libraries (test_adjacencymatrix ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
target_link_libraries (test_adjacencymatrixindexer ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_program(VALGRIND valgrind)
set(VALGRIND_OPTIONS --leak-check=full --error-exitcode=1000)

enable_testing()

add_test(AdjacencyMatrix        ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrix)
add_test(AdjacencyMatrixIndexer ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrixindexer)
