cmake_minimum_required(VERSION 3.14)

project(demo)

set(CMAKE_CXX_STANDARD 14)


######################### source files ##################################
add_library(
   serilization
   serializer.cpp
   serializer.hpp
   deserializer.cpp
   deserializer.hpp
   
)

######################### samples ##################################

add_executable(test test.cpp)
target_link_libraries(test serilization)
