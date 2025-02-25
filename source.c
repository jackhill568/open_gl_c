#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <linmath/linmath.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "camera.h"
#include "shader.h"
#include "window.h"

Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLfloat vertices[] = {
    // Positions         // Colors
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Bottom-left-back (Red)
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-right-back (Green)
    0.5f,  0.5f,  -0.5f, 0.0f, 1.0f, 1.0f, // Top-right-back (Blue)
    -0.5f, 0.5f,  -0.5f, 1.0f, 0.0f, 1.0f, // Top-left-back (Yellow)

    -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f, // Bottom-left-front (Purple)
    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // Bottom-right-front (Cyan)
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // Top-right-front (White)
    -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 0.0f  // Top-left-front (Black)
};

GLuint indices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    3, 2, 6, 3, 6, 7, // Top face
    0, 1, 5, 0, 5, 4, // Bottom face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
};

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
  if (!window_init(&window, 2560, 1440, "Cunt Window")) {
    glfwTerminate();
    return -1;
  }
  glfwSetKeyCallback(window.handle, key_callback);

  Shader shader;
  shader_init(&shader, "../shader.vert", "../shader.frag");
  camera_init(&camera);

  // Setup VAO/VBO
  GLuint vao, vbo, EBO;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Get uniform locations
  glEnable(GL_DEPTH_TEST);
  glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window.handle, mouse_callback);

  mat4x4 model;
  mat4x4_identity(model);
  mat4x4 projection;
  mat4x4_perspective(projection, M_PI / 4,
                     ((float)window.width / (float)window.height), 0.1f,
                     100.0f);
  mat4x4 view;

  while (!glfwWindowShouldClose(window.handle)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    shader_use(&shader);
    // Render to screen
    glViewport(0, 0, window.width, window.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    key_process(window);

    // incase of resizeing
    int width, height;
    glfwGetFramebufferSize(window.handle, &width, &height);
    float aspect = (float)width / (float)height;
    mat4x4_perspective(projection, M_PI / 4, aspect, 0.1f, 100.0f);

    camera_get_view_matrix(&camera, view);

    shader_set_mat4(&shader, "model", (float *)model);
    shader_set_mat4(&shader, "view", (float *)view);
    shader_set_mat4(&shader, "proj", (float *)projection);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window.handle);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &EBO);
  // glDeleteProgram(shaderProgram);
  window_cleanup(&window);
  glfwTerminate();
  return 0;
}
