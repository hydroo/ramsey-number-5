cmake_minimum_required (VERSION 3.9.1)
project (test-5-new)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(test-5-new test-5-new.cpp test-5-check.cpp)

configure_file(test-5-build.sh test-5-build.sh COPYONLY)

set_source_files_properties(test_adjacencymatrix.cpp test_adjacencymatrixindexer.cpp PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
target_link_libraries(test_adjacencymatrix ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
target_link_libraries(test_adjacencymatrixindexer ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_program(VALGRIND valgrind)
set(VALGRIND_OPTIONS --leak-check=full --error-exitcode=1000)

enable_testing()

add_test(AdjacencyMatrix        ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrix)
add_test(AdjacencyMatrixIndexer ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrixindexer)
