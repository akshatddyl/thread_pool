cmake_minimum_required(VERSION 3.20)
project(CppThreadPool VERSION 1.0.0 LANGUAGES CXX)

# --- Options ---
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(BUILD_TESTS "Build unit tests" ON)

# --- Compiler Standard & Warnings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common warning flags
add_compile_options(
    -Wall -Wextra -Wpedantic
    $<$<CXX_COMPILER_ID:GNU>:-Wconversion -Wsign-conversion>
    $<$<CXX_COMPILER_ID:Clang>:-Wconversion -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# --- Sanitizers ---
if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

if(ENABLE_TSAN)
    add_compile_options(-fsanitize=thread -g)
    add_link_options(-fsanitize=thread)
endif()

# --- Library ---
add_library(threadpool_lib
    src/blocking_queue.cpp
    src/thread_pool.cpp
)
target_include_directories(threadpool_lib PUBLIC include)

# --- Tests ---
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # Prevent GoogleTest from overriding our compiler flags
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(threadpool_tests tests/thread_pool_tests.cpp)
    target_link_libraries(threadpool_tests PRIVATE threadpool_lib GTest::gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(threadpool_tests)
endif()
