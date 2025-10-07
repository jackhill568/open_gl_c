cmake_minimum_required(VERSION 3.15)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(help)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)



include_directories("include/")

set(ASSIMP_ROOT "$ENV{HOME}/.assimp")  
link_directories(${ASSIMP_ROOT}/bin)
include_directories(${ASSIMP_ROOT}/include)

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
    src/Texture.c
    src/GameObject.c
)
add_executable(Help ${SRC_FILES})
target_link_libraries(Help OpenGL::GL assimp glfw m)
