cmake_minimum_required(VERSION 3.20)
project(Green_Scheduling_Distributed_Flowshop_Problem)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(Green_Scheduling_Distributed_Flowshop_Problem
        include/defines.hpp
        include/Factory.hpp
        include/Job.hpp
        include/Machine.hpp
        include/Instance.hpp
        include/read.hpp
        include/Solution.hpp
        source/Factory.cpp
        source/Job.cpp
        source/Machine.cpp
        source/main.cpp
        source/Instance.cpp
        source/read.cpp
        source/Solution.cpp)
