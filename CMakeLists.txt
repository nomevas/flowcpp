cmake_minimum_required(VERSION 3.3)
project(flow)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(flow ${SOURCE_FILES})
add_executable(app_store app_store.cpp)