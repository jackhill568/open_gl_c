

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

char *readShaderFile(char *filename) {
  long file_size;
  char *buffer;
  FILE *file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Could not open file\n");
    return "";
  }
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);
  buffer = (char *)malloc(file_size + 1);
  if (buffer == NULL) {
    printf("Memory allocation failed\n");
    fclose(file);
    return "";
  }
  fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';
  fclose(file);
  return buffer;
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
  // vertex shader stuff
  const char *vertexSource = readShaderFile("../shader.vert");
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, NULL);
  glCompileShader(vertexShader);
  checkCompileErrors(vertexShader, "VERTEX");

  // fragment shader stuff
  const char *fragmentSource = readShaderFile("../shader.frag");
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader);
  checkCompileErrors(fragmentShader, "FRAGMENT");

  // attach the shaders together
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glBindFragDataLocation(shaderProgram, 0, "fragment");

  glLinkProgram(shaderProgram);

  glUseProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  free(vertexSource);
  free(fragmentSource);

  return shaderProgram;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

// key callback gets key input cool ig, needs to be 'initialized'
static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

GLFWwindow *inits(void) {
  // Initialize GLFW
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return NULL;
  }
  GLFWwindow *window = glfwCreateWindow(800, 800, "Cunt Window", NULL, NULL);
  if (!window) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }
  glfwMakeContextCurrent(window);
  // Load OpenGL functions using GLAD
  if (!gladLoadGL(glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    glfwTerminate();
    return NULL;
  }
  return window;
}

int main() {
  GLFWwindow *window = inits();
  GLuint shaderProgram = setupShaders();
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, 100 * 2 * sizeof(GLfloat), {},
  // GL_STATIC_DRAW);

  GLint posAttrib = glGetAttribLocation(shaderProgram, "vPos");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
                        0);
  glEnableVertexAttribArray(posAttrib);

  GLuint timeAttibute = glGetUniformLocation(shaderProgram, "time");
  glUniform1f(timeAttibute, 0.);

  GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColour");
  glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);

  // initialize the glfw callback
  glfwSetErrorCallback(error_callback);

  // OpenGL settings
  glViewport(0, 0, 800, 800);

  float startTime = glfwGetTime();
  // initialize key callbak
  glfwSetKeyCallback(window, key_callback);
  glfwSwapInterval(1);
  // Main loop
  while (!glfwWindowShouldClose(window)) {
    int width, height;

    float currentTime = glfwGetTime();
    float timeElapsed = currentTime - startTime;

    glfwGetFramebufferSize(window, &width, &height);
    float aspectRatio = (float)width / (float)height;
    // Render
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glUniform1f(timeAttibute, timeElapsed);
    glUniform3f(uniColor, (cos(timeElapsed * 4.0f) + 1.0f) / 2.0f,
                (sin(timeElapsed * 4.0f) + 1.0f) / 2.0f, 0.0f);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    // glClearColor((sin(timeElapsed * 4.0f) + 1.0f) / 2.0f, 0.3f,
    // (cos(timeElapsed * 4.0f) + 1.0f) / 2.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 102);
    // Swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  // Clean up and exit
  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
