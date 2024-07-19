cmake_minimum_required (VERSION 3.30)

project(test-8)

enable_language(CXX)

find_package(GTest)
find_package(Threads REQUIRED)
find_package(Boost 1.74 REQUIRED COMPONENTS container)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic") # Size=s64 and [] operator result in tons of implicit conversion warnings
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wconversion")

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS_DEBUG      "-DDEBUG -g -Og -fsanitize=address -fsanitize=undefined -fsanitize=leak ")
else() # Darwin and Windows
    set(CMAKE_CXX_FLAGS_DEBUG      "-DDEBUG -g -Og -fsanitize=address -fsanitize=undefined")
endif()
set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG -O3 -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -O3 -march=native -mtune=native -g -fno-omit-frame-pointer")

add_executable(test-8 test-8.cpp)
target_include_directories(test-8 PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(test-8 PRIVATE Boost::container)

configure_file(build.sh build.sh COPYONLY)
configure_file(bench.sh bench.sh COPYONLY)

#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=1")
#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=2")
#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=3")
#set_source_files_properties(test-8.cpp PROPERTIES COMPILE_FLAGS "-DR5_VERBOSE=4")

if (GTest_FOUND)
    set_source_files_properties(test_adjacencymatrix.cpp test_adjacencymatrixindexer.cpp test_test-8 PROPERTIES COMPILE_FLAGS "-DR5_GTEST")

    add_executable(test_adjacencymatrix test_adjacencymatrix.cpp)
    target_include_directories(test_adjacencymatrix PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(test_adjacencymatrix PRIVATE ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

    add_executable(test_adjacencymatrixindexer test_adjacencymatrixindexer.cpp)
    target_include_directories(test_adjacencymatrixindexer PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(test_adjacencymatrixindexer PRIVATE ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

    add_executable(test_test-8 test_test-8.cpp)
    target_include_directories(test_test-8 PRIVATE ${Boost_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS} )
    target_link_libraries(test_test-8 PRIVATE Boost::container ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

    find_program(VALGRIND valgrind)
    if (VALGRIND)
        set(VALGRIND_WITH_OPTIONS "${VALGRIND} --leak-check=full --error-exitcode=1000")
    else()
        set(VALGRIND_WITH_OPTIONS "")
        message(STATUS "Valgrind not found. Skipping for tests.")
    endif()

    enable_testing()

    add_test(AdjacencyMatrix        ${VALGRIND_WITH_OPTIONS} ./test_adjacencymatrix)
    add_test(AdjacencyMatrixIndexer ${VALGRIND_WITH_OPTIONS} ./test_adjacencymatrixindexer)
    add_test(Test-8                 ${VALGRIND_WITH_OPTIONS} ./test_test-8)
else()
    message(STATUS "GTest not found. Skipping tests.")
endif()