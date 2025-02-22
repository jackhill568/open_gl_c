#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <linmath/linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
int WIDTH = 2560;
int HEIGHT = 1440;

GLfloat vertices[] = {
    // Positions         // Colors
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left-back (Red)
    0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-right-back (Green)
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, // Top-right-back (Blue)
    -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 1.0f, // Top-left-back (Yellow)

    -0.5f, -0.5f, 0.5f,  1.0f, 1.0f, 1.0f, // Bottom-left-front (Purple)
    0.5f,  -0.5f, 0.5f,  1.0f, 1.0f, 1.0f, // Bottom-right-front (Cyan)
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, // Top-right-front (White)
    -0.5f, 0.5f,  0.5f,  1.0f, 1.0f, 1.0f  // Top-left-front (Black)
};

GLuint indices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    3, 2, 6, 3, 6, 7, // Top face
    0, 1, 5, 0, 5, 4, // Bottom face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
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

void create_look_rotation(mat4x4 matrix, vec3 R, vec3 U, vec3 D) {
  matrix[0][0] = R[0];
  matrix[1][0] = R[1];
  matrix[2][0] = R[2];
  matrix[3][0] = 0.0f;

  matrix[0][1] = U[0];
  matrix[1][1] = U[1];
  matrix[2][1] = U[2];
  matrix[3][1] = 0.0f;

  matrix[0][2] = D[0];
  matrix[1][2] = D[1];
  matrix[2][2] = D[2];
  matrix[3][2] = 0.0f;

  matrix[0][3] = 0.0f;
  matrix[1][3] = 0.0f;
  matrix[2][3] = 0.0f;
  matrix[3][3] = 1.0f;
}

void create_look_translation(mat4x4 matrix, vec3 camPos) {
  mat4x4_identity(matrix);
  matrix[3][0] = -camPos[0];
  matrix[3][1] = -camPos[1];
  matrix[3][2] = -camPos[2];
}

void get_view_mat(mat4x4 view) {
  const float radius = 10.0f;
  float camX = sin(glfwGetTime()) * radius;
  float camZ = cos(glfwGetTime()) * radius;

  vec3 cameraPos = {camX, 0.0f, camZ};
  vec3 cameraTarget = {0.0f, 0.0f, 0.0f};
  vec3 cameraDirection;
  vec3_sub(cameraDirection, cameraPos, cameraTarget);
  vec3_norm(cameraDirection, cameraDirection);

  vec3 up = {0.0f, 1.0f, 0.0f};
  vec3 cameraRight;
  vec3_mul_cross(cameraRight, up, cameraDirection);
  vec3_norm(cameraRight, cameraRight);

  vec3 cameraUp;
  vec3_mul_cross(cameraUp, cameraDirection, cameraRight);

  mat4x4 cameraRotation;
  create_look_rotation(cameraRotation, cameraRight, cameraUp, cameraDirection);
  // mat4x4_invert(cameraRotation, cameraRotation);

  mat4x4 cameraTranslation;
  // mat4x4_invert(cameraTranslation, cameraTranslation);
  create_look_translation(cameraTranslation, cameraPos);

  mat4x4_mul(view, cameraRotation, cameraTranslation);
}

int main() {
  GLFWwindow *window = initOpenGL();
  if (!window)
    return -1;

  GLuint shaderProgram = setupShaders();

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
  mat4x4 model;
  mat4x4_identity(model);

  mat4x4 projection;
  mat4x4_perspective(projection, M_PI / 4, (float)(WIDTH / HEIGHT), 0.1f,
                     100.0f);

  int modelLoc = glGetUniformLocation(shaderProgram, "model");

  int viewLoc = glGetUniformLocation(shaderProgram, "view");

  int projLoc = glGetUniformLocation(shaderProgram, "proj");

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Texture coord attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Get uniform locations
  glUseProgram(shaderProgram);
  glEnable(GL_DEPTH_TEST);
  double lastUpdateTime = glfwGetTime();
  const double updateInterval = 0.015;
  while (!glfwWindowShouldClose(window)) {
    double current_time = glfwGetTime();
    mat4x4 view;

    // Render to screen
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (current_time - lastUpdateTime >= updateInterval) {
      get_view_mat(view);
      mat4x4_rotate(model, model, 0.25f, 0.5f, 1.0f,
                    (float)(current_time - lastUpdateTime) * 1.0f);
      lastUpdateTime = current_time;
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
