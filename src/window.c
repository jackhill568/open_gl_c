#include "window.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

bool window_init(Window *window, int width, int height, const char *title) {

  if (!glfwInit()) {
    printf("Error: Failed to initialize GLFW\n");
    return false;
  }

  window->width = width;
  window->height = height;
  window->title = title;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!window->handle) {
    printf("Error: Failed to create GLFW window\n");
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window->handle);

  if (!gladLoadGL(glfwGetProcAddress)) {
    printf("Error: Failed to initialize GLAD\n");
    glfwTerminate();
    return false;
  }
  return true;
}
void window_cleanup(Window *window) {
  if (window->handle) {
    glfwDestroyWindow(window->handle);
  }
}
