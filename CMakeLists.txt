cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

add_executable(untitled defs.h rtos_api.h sys.h event.cpp global.cpp os.cpp resource.cpp task.cpp test.cpp)
