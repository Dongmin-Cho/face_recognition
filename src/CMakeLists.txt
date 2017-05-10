cmake_minimum_required(VERSION 2.8.12)
project(src)

# it needs is the dlib source code folder and it will take care of everything.
include(../dlib/cmake)

macro(add_example name)
   add_executable(${name} ${name}.cpp)
   target_link_libraries(${name} dlib::dlib )
endmacro()
add_example(extract_vector)
add_example(transpose_vector)
