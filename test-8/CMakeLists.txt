cmake_minimum_required (VERSION 3.13)
project (test-8 LANGUAGES CXX CUDA)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS graph)
include_directories(${GTEST_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "-Og -DDEBUG -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native")
#set(CMAKE_CXX_FLAGS_RELEASE "-O2 -mtune=native -fno-omit-frame-pointer -g")

set(CMAKE_CUDA_FLAGS         "-Xcompiler \"${CMAKE_CXX_FLAGS}\""        )
set(CMAKE_CUDA_FLAGS_DEBUG   "-Xcompiler \"${CMAKE_CXX_FLAGS_DEBUG}\""  )
set(CMAKE_CUDA_FLAGS_RELEASE "-Xcompiler \"${CMAKE_CXX_FLAGS_RELEASE}\"")

add_executable(test-8 test-8.cu)
set_property(TARGET test-8 PROPERTY CUDA_STANDARD 14)

configure_file(build.sh build.sh COPYONLY)

set_source_files_properties(test_adjacencymatrix.cpp test_adjacencymatrixindexer.cpp test_test-8 PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

#set_source_files_properties(test-8.cu PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=1")
#set_source_files_properties(test-8.cu PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=2")
#set_source_files_properties(test-8.cu PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=3")

add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
target_link_libraries(test_adjacencymatrix ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
target_link_libraries(test_adjacencymatrixindexer ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(test_test-8 test_test-8.cu)
target_link_libraries(test_test-8 ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

find_program(VALGRIND valgrind)
set(VALGRIND_OPTIONS --leak-check=full --error-exitcode=1000)

enable_testing()

add_test(AdjacencyMatrix        ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrix)
add_test(AdjacencyMatrixIndexer ${VALGRIND} ${VALGRIND_OPTIONS} ./test_adjacencymatrixindexer)
add_test(Test-8                 ${VALGRIND} ${VALGRIND_OPTIONS} ./test_test-8)
