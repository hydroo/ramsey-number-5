cmake_minimum_required (VERSION 3.9.1)
project (test-7)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native -fno-omit-frame-pointer -g")

add_executable(test-7 test-7.cpp check.cpp)

configure_file(build.sh build.sh COPYONLY)
configure_file(test-7.sh test-7.sh COPYONLY)

set_source_files_properties(test_adjacencymatrix.cpp test_adjacencymatrixindexer.cpp test_test-7.cpp PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

#set_source_files_properties(test-7.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE")

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
target_link_libraries(test_adjacencymatrix ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
target_link_libraries(test_adjacencymatrixindexer ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_test-7 test_test-7.cpp)
target_link_libraries(test_test-7 ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_program(VALGRIND valgrind)
set(VALGRIND_OPTIONS --leak-check=full --error-exitcode=1000)

enable_testing()

add_test(AdjacencyMatrix        ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrix)
add_test(AdjacencyMatrixIndexer ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrixindexer)
add_test(Test-7                 ${VALGRIND} ${VALGRIND_OPTIONS} ./test_test-7)
