cmake_minimum_required(VERSION 3.1)
project(nothread CXX ASM)

# General configuration

include_directories(include)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fdiagnostics-color=always -ggdb3 -pedantic")
else()
	message("[Warning]: You are building without gnu c++ compiler, you may need to tune some things")
endif()

# Build lib

add_library(nothread src/asm-helper-x86_64.s)

# Build demos

add_subdirectory(demos)

# Tests

add_library(catch INTERFACE)
target_include_directories(catch INTERFACE extern/catch)

set(TEST_SRC test/main.cpp
             test/test-context.cpp
             test/test-coroutine.cpp
             test/test-allocs.cpp
             test/test-allocs-2.cpp
             test/test-abort.cpp)

add_executable(nothread-test ${TEST_SRC})
target_link_libraries(nothread-test catch nothread)
enable_testing()
add_test(NAME nothread-test COMMAND nothread-test)


                  
