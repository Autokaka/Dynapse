cmake_minimum_required(VERSION 3.8.0)
project(dynapse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN YES)

option(DYNAPSE_CORE "Build dynapse CORE functionality only." OFF)

# dynapse
add_library(dynapse INTERFACE)
target_include_directories(dynapse INTERFACE include)
if (DYNAPSE_CORE)
  target_compile_definitions(dynapse INTERFACE -DDYNAPSE_CORE)
endif()

# test
add_executable(test test.cc)
target_link_libraries(test dynapse)
