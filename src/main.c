#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include <GL/gl.h>
#include <linmath/linmath.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "GameObject.h"
#include "Model.h"
#include "camera.h"
#include "shader.h"
#include "window.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <stdio.h>
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
GameObject sha;

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
      lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  camera_process_mouse(&camera, xoffset, yoffset, true);
}

int main() {
  Window window;
  if (!window_init(&window, 1920, 1080, "Window")) {
    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window.handle, key_callback);

  Shader shader;
  compile_shader(&shader, "../shaders/shader.vert.glsl", "../shaders/shader.frag.glsl");
  camera_init(&camera);

  loadModel("../assets/smallShark.fbx", &sha.sprite);
  vec3_set(sha.position, (vec3){0, 0, 0});
  vec3_set(sha.front, (vec3){0, 0, 1});
  sha.yaw = 90;
  sha.pitch = 0;
  obj_update_vectors(&sha);

  glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window.handle, mouse_callback);

  mat4x4 model;
  mat4x4_identity(model);
  mat4x4 projection;
  mat4x4_perspective(projection, M_PI / 4,
                     ((float)window.width / (float)window.height), 0.1f,
                     1600.0f);
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
    glViewport(0, 0, window.width, window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.2f, 1.0f);

    key_process(window);

    int width, height;
    glfwGetFramebufferSize(window.handle, &width, &height);
    float aspect = (float)width / (float)height;
    mat4x4_perspective(projection, M_PI / 4, aspect, 0.1f, 1000.0f);

    shader_set_mat4(&shader, "view", (float *)view);
    shader_set_mat4(&shader, "proj", (float *)projection);

    shader_set_mat4(&shader, "viewpos", (float *)camera.position);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
      printf("OpenGL error: %d\n", err);
    }
    draw_obj(&sha, &shader);
    camera_get_view_matrix(&camera, view);
    glfwSwapBuffers(window.handle);
    glfwPollEvents();
  }

  glDeleteProgram(shader.ID);
  clean_model(&sha.sprite);

  window_cleanup(&window);
  glfwTerminate();
  return 0;
}
