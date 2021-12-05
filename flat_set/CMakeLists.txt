project(mipt-set)
cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 17)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Og")

add_subdirectory(mipt)

add_executable(target main.cpp)
target_include_directories(target PUBLIC .)
target_link_libraries(target lib_all)

