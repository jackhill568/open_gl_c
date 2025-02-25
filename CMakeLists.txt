cmake_minimum_required(VERSION 3.15)

project(OpenglThing)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(OpenglThing source.c /home/ja/CompSci/CThings/open_gl_c/src/gl.c)
include_directories("/home/ja/CompSci/CThings/open_gl_c/include/")

target_link_libraries(OpenglThing OpenGL::GL glfw m)
