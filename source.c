#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math.h>
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

GLuint createGameOfLifeTexture(int width, int height) {
  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT,
               NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  return textureID;
}

void initializeGrid(GLuint textureID, int width, int height) {
  float *data = (float *)malloc(width * height * sizeof(float));
  for (int i = 0; i < width * height; i++) {
    data[i] = (float)(rand() % 2);
  }
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_FLOAT,
                  data);
  free(data);
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

  // Position attribute
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Create textures and FBOs for ping-pong
  GLuint textures[2];
  textures[0] = createGameOfLifeTexture(WIDTH, HEIGHT);
  textures[1] = createGameOfLifeTexture(WIDTH, HEIGHT);
  initializeGrid(textures[0], WIDTH, HEIGHT);

  GLuint fbos[2];
  glGenFramebuffers(2, fbos);
  for (int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           textures[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      printf("Framebuffer is not complete!\n");
      return -1;
    }
  }

  // Get uniform locations
  glUseProgram(shaderProgram);
  GLuint texLoc = glGetUniformLocation(shaderProgram, "currentstate");
  GLuint resLoc = glGetUniformLocation(shaderProgram, "resolution");
  GLuint mosLoc = glGetUniformLocation(shaderProgram, "mousePos");
  glUniform2f(resLoc, WIDTH, HEIGHT);

  double lastUpdateTime = glfwGetTime();
  const double updateInterval = 0.0001;
  int currentTexture = 0;
  double xpos, ypos;
  while (!glfwWindowShouldClose(window)) {
    double currentTime = glfwGetTime();

    if (currentTime - lastUpdateTime >= updateInterval) {

      glfwGetCursorPos(window, &xpos, &ypos);
      glUniform2f(mosLoc, xpos, ypos);
      // Update state
      glBindFramebuffer(GL_FRAMEBUFFER, fbos[1 - currentTexture]);
      glViewport(0, 0, WIDTH, HEIGHT);
      glUseProgram(shaderProgram);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
      glUniform1i(texLoc, 0);

      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

      // Swap textures
      currentTexture = 1 - currentTexture;
      lastUpdateTime = currentTime;
    }

    // Render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[currentTexture]);
    glUniform1i(texLoc, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteFramebuffers(2, fbos);
  glDeleteTextures(2, textures);
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
