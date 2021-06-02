cmake_minimum_required(VERSION 3.13)
project(TSP_Implementation)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

include_directories(
        include
)

set(SOURCE_FILES
        src/tsp_setup.cpp
        src/tsp.cpp
        )

add_executable(tsp ${SOURCE_FILES} main.cpp)

set(SOURCES_FILES_TESTS
        test/test_tsp.cpp
        )

add_executable(tsp__test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

target_include_directories(tsp__test PUBLIC
        ../googletest-master/googlemock/include
        ../../googletest-master/googletest/include
        )

add_subdirectory(googletest-master)

target_link_libraries(tsp__test gmock)