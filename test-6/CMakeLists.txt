cmake_minimum_required (VERSION 3.9.1)
project (test-6)

set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_executable(test-6 adjacencymatrix.cpp main.cpp prereqs.cpp)

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp adjacencymatrix.cpp prereqs.cpp )

enable_testing()

add_test(AdjacencyMatrix test_adjacencymatrix)
