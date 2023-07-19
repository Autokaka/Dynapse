cmake_minimum_required(VERSION 3.8.0)
project(dynapse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN YES)

# dynapse
file(GLOB_RECURSE SOURCE "${PROJECT_SOURCE_DIR}/src/*.cc")
add_library(dynapse STATIC ${SOURCE})
target_include_directories(dynapse 
  PUBLIC "${PROJECT_SOURCE_DIR}/include"
  PRIVATE "${PROJECT_SOURCE_DIR}/src"
)

# test
add_executable(test test.cc)
target_link_libraries(test dynapse)
