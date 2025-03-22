#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "src/shape.h"
#include <GL/gl.h>
#include <linmath/linmath.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "src/camera.h"
#include "src/shader.h"
#include "src/window.h"
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
    0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
    0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
    -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
    0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
    -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
    0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
    -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
    -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
    1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
    -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
    1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

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
unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    printf("texture fail");
    stbi_image_free(data);
  }

  return textureID;
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

  GLuint vbo;
  GLuint vao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)0);

  // Normals
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
  // texure coords
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                        (void *)(6 * sizeof(GLfloat)));

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int texture = loadTexture("../assests/container2.png");
  unsigned int spec_map = loadTexture("../assests/container2_specular.png");

  ShapeBuffer lightCube;
  init_shapes(&lightCube);
  Shape LightSource;
  make_shape(&LightSource, (vec3){0.0f, 0.0f, 0.0f}, "cube",
             (float[]){1.0f, 1.0f, 1.0f});

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
  glUniform1i(glGetUniformLocation(shader.ID, "material.diffuse"), 0);
  glUniform1i(glGetUniformLocation(shader.ID, "material.specular"), 1);
  glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), 32.0f);

  while (!glfwWindowShouldClose(window.handle)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    vec3_dup(LightSource.pos,
             (vec3){-2 * cos(currentFrame), 8 * sin(currentFrame),
                    8 * cos(currentFrame) + 1});
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, spec_map);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwSwapBuffers(window.handle);
    glfwPollEvents();
  }

  clean_buffers(&lightCube);
  // glDeleteProgram(shaderProgram);

  window_cleanup(&window);
  glfwTerminate();
  return 0;
}
