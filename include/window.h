#ifndef WINDOW_H
#define WINDOW_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  GLFWwindow *handle;
  int width;
  int height;
  const char *title;
} Window;

bool window_init(Window *window, int width, int height, const char *title);

void window_cleanup(Window *window);

#endif
