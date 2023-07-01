cmake_minimum_required(VERSION 3.20)
project(Green_Scheduling_Distributed_Flowshop_Problem)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "-O3")
SET(CMAKE_C_FLAGS "-O3")

include_directories(include)

add_executable(Green_Scheduling_Distributed_Flowshop_Problem
        include/defines.hpp
        include/Factory.hpp
        include/Job.hpp
        include/Machine.hpp
        include/Solution.hpp
        include/Problem.hpp
        include/read.hpp
        source/Factory.cpp
        source/Job.cpp
        source/Machine.cpp
        source/main.cpp
        source/Problem.cpp
        source/read.cpp
        source/Solution.cpp
        include/Util.h
        source/Util.cpp)
