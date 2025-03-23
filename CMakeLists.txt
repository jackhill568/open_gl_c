cmake_minimum_required(VERSION 3.15)

project(OpenglThing)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/ja/CompSci/CThings/open_gl_c/include/assimp-install/")

find_package(assimp REQUIRED)

add_executable(OpenglThing source.c ../src/gl.c)
include_directories("include/")
target_link_libraries(OpenglThing OpenGL::GL assimp::assimp glfw m)
