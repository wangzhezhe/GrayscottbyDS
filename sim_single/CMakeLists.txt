
cmake_minimum_required (VERSION 3.14)
project (sim_single C CXX)
set (CMAKE_CXX_STANDARD 14)

add_executable(sim_single
  main.cpp
  settings_single.cpp
  sim_single.cpp)