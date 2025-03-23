#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "src/shape.h"
#include <GL/gl.h>
#include <linmath/linmath.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "src/Model.h"
#include "src/camera.h"
#include "src/shader.h"
#include "src/window.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdio.h>
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}

void key_process(Window window) {
  if (glfwGetKey(window.handle, GLFW_KEY_W) == GLFW_PRESS) {
    camera_process_keyboard(&camera, GLFW_KEY_W, deltaTime);
  }
  if (glfwGetKey(window.handle, GLFW_KEY_S) == GLFW_PRESS) {
    camera_process_keyboard(&camera, GLFW_KEY_S, deltaTime);
  }
  if (glfwGetKey(window.handle, GLFW_KEY_A) == GLFW_PRESS) {
    camera_process_keyboard(&camera, GLFW_KEY_A, deltaTime);
  }
  if (glfwGetKey(window.handle, GLFW_KEY_D) == GLFW_PRESS) {
    camera_process_keyboard(&camera, GLFW_KEY_D, deltaTime);
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
    return;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // Reversed since y-coordinates range from bottom to top
  lastX = xpos;
  lastY = ypos;

  camera_process_mouse(&camera, xoffset, yoffset, true);
}

int main() {
  Window window;
  if (!window_init(&window, 2560, 1440, "Window")) {
    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window.handle, key_callback);

  Shader shader;
  shader_init(&shader, "../shaders/shader.vert", "../shaders/shader.frag");
  camera_init(&camera);

  Shader lightShader;
  shader_init(&lightShader, "../shaders/shader.vert",
              "../shaders/lightShader.frag");

  ShapeBuffer lightCube;
  init_shapes(&lightCube);
  Shape LightSource;
  make_shape(&LightSource, (vec3){0.0f, 0.0f, 0.0f}, "cube",
             (float[]){1.0f, 1.0f, 1.0f});
  Model shark;
  loadModel("../assets/smallShark.fbx", &shark);

  glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window.handle, mouse_callback);

  mat4x4 model;
  mat4x4_identity(model);
  mat4x4 projection;
  mat4x4_perspective(projection, M_PI / 4,
                     ((float)window.width / (float)window.height), 0.1f,
                     600.0f);
  mat4x4 view;
  glEnable(GL_DEPTH_TEST);

  shader_use(&shader);
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    printf("OpenGL error: %d\n", err);
  }

  while (!glfwWindowShouldClose(window.handle)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    vec3_dup(LightSource.pos, (vec3){-2 * cos(currentFrame), 8 * sin(currentFrame), 8 * cos(currentFrame) + 1});
    //  Render to screen
    glViewport(0, 0, window.width, window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    key_process(window);

    // incase of resizeing
    int width, height;
    glfwGetFramebufferSize(window.handle, &width, &height);
    float aspect = (float)width / (float)height;
    mat4x4_perspective(projection, M_PI / 4, aspect, 0.1f, 100.0f);

    camera_get_view_matrix(&camera, view);

    shader_set_mat4(&shader, "view", (float *)view);
    shader_set_mat4(&shader, "proj", (float *)projection);
    shader_set_mat4(&shader, "licol", (float *)(vec3){1.0f, 1.0f, 1.0f});
    shader_set_mat4(&shader, "lipos", (float *)LightSource.pos);

    shader_set_mat4(&shader, "viewpos", (float *)camera.position);

    shader_use(&lightShader);
    shader_set_mat4(&lightShader, "view", (float *)view);
    shader_set_mat4(&lightShader, "proj", (float *)projection);
    draw_shape(&LightSource, &lightCube, &lightShader);

    shader_use(&shader);
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      printf("OpenGL error: %d\n", err);
    }
    mat4x4 model;
    mat4x4_translate(model, 1.0f, 1.0f, 1.0f);
    shader_set_mat4(&shader, "model", (float *)model);

    DrawModel(&shader, &shark);

    glfwSwapBuffers(window.handle);
    glfwPollEvents();
  }

  clean_buffers(&lightCube);
  // glDeleteProgram(shaderProgram);
  clean_model(&shark);
  window_cleanup(&window);
  glfwTerminate();
  return 0;
}
