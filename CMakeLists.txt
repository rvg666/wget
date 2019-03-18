cmake_minimum_required(VERSION 3.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (srcs
  my_wget.h
  my_wget.cpp)

add_executable(my_wget ${srcs})

target_compile_definitions(my_wget
PRIVATE
    __POSIX
)
