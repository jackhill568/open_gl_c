#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <linmath/linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
int WIDTH = 2560;
int HEIGHT = 1440;

GLfloat vertices[] = {
    -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, // Top-left
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
    1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    1.0f,  1.0f,  0.0f, 1.0f, 1.0f  // Top-right
};

char *readShaderFile(char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    printf("Could not open file %s\n", filename);
    return "";
  }
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *buffer = malloc(size + 1);
  fread(buffer, 1, size, file);
  buffer[size] = '\0';
  fclose(file);
  return buffer;
}
static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void checkCompileErrors(GLuint shader, char *type) {
  GLint success;
  GLchar infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n", type,
             infoLog);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n", type, infoLog);
    }
  }
}

GLuint setupShaders(void) {
  const char *vertexSource = readShaderFile("../shader.vert");
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  checkCompileErrors(vertexShader, "VERTEX");

  const char *fragmentSource = readShaderFile("../shader.frag");
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  checkCompileErrors(fragmentShader, "FRAGMENT");

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  checkCompileErrors(shaderProgram, "PROGRAM");

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  free((void *)vertexSource);
  free((void *)fragmentSource);

  return shaderProgram;
}

GLFWwindow *initOpenGL() {
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return NULL;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "Cunt window", NULL, NULL);
  if (!window) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGL(glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return NULL;
  }

  glfwSetKeyCallback(window, key_callback);
  return window;
}

float *flatten_mat4x4(mat4x4 matrix) {
  float *outArray = malloc(16 * sizeof(float));
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      outArray[i * 4 + j] = matrix[i][j];
    }
  }
  return outArray;
}

int main() {
  GLFWwindow *window = initOpenGL();
  if (!window)
    return -1;

  GLuint shaderProgram = setupShaders();

  // Setup VAO/VBO
  GLuint vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  mat4x4 trans;

  GLuint unitrans = glGetUniformLocation(shaderProgram, "trans");
  glUniformMatrix4fv(unitrans, 1, GL_FALSE, flatten_mat4x4(trans));

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Get uniform locations
  glUseProgram(shaderProgram);

  while (!glfwWindowShouldClose(window)) {

    // Render to screen
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
