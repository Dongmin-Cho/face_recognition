cmake_minimum_required(VERSION 2.8.12)
project(src)

# it needs is the dlib source code folder and it will take care of everything.
include(../dlib/cmake)

macro(add_example name)
   add_executable(${name} ${name}.cpp)
   target_link_libraries(${name} dlib::dlib )
endmacro()
add_example(check_attendance)
add_example(test_desc)
add_example(tracking)
add_example(testcv)
add_example(test_vector)

#add_example(crop)
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(crop crop.cpp crop_face_img.cpp)
target_link_libraries(crop ${OpenCV_LIBS} dlib::dlib)

