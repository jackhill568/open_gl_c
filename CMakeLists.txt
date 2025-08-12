cmake_minimum_required(VERSION 3.15)

project(Help)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/home/ja/CompSci/CThings/open_gl_c/include/assimp-install/")

find_package(assimp REQUIRED)

include_directories("include/")

set(SRC_FILES
    src/camera.c
    src/gl.c
    src/helpers.c
    src/main.c
    src/Mesh.c
    src/Model.c
    src/shader.c
    src/shape.c
    src/window.c
)
add_executable(Help ${SRC_FILES})
target_link_libraries(Help OpenGL::GL assimp::assimp glfw m)
