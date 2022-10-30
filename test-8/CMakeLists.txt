cmake_minimum_required (VERSION 3.13)

project(test-8)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
#find_package(Boost COMPONENTS graph)
include_directories(${GTEST_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wconversion")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native  -mtune=native -fno-omit-frame-pointer -g")

## gprof
#set(CMAKE_EXE_LINKER_FLAGS "-pg")
#set(CMAKE_CXX_FLAGS        "-pg ${CMAKE_CXX_FLAGS}")

add_executable(test-8 test-8.cpp)

configure_file(build.sh build.sh COPYONLY)

set_source_files_properties(test_adjacencymatrix.cpp test_adjacencymatrixindexer.cpp test_test-8 PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=1")
#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=2")
#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=3")

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
target_link_libraries(test_adjacencymatrix ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
target_link_libraries(test_adjacencymatrixindexer ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_test-8 test_test-8.cpp)
target_link_libraries(test_test-8 ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_program(VALGRIND valgrind)
set(VALGRIND_OPTIONS --leak-check=full --error-exitcode=1000)

enable_testing()

add_test(AdjacencyMatrix        ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrix)
add_test(AdjacencyMatrixIndexer ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrixindexer)
add_test(Test-8                 ${VALGRIND} ${VALGRIND_OPTIONS} ./test_test-8)
