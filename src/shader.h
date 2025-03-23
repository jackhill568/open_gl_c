
#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include "linmath/linmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int model;
  int view;
  int projection;
  int lightamb;
  int lightdiff;
  int lightspec;
  int lightPos;
  int viewPos;
  int constant;
  int linear;
  int quadratic;
} UniformLocations;

typedef struct {
  GLuint ID;
  UniformLocations Locs;
} Shader;

void checkCompileErrors(GLuint shader, char *type) {
  GLint success;
  GLchar infoLog[1024];
  if (strcmp(type, "PROGRAM") != 0) {
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
char *readShaderFile(const char *filename) {

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
void shader_init(Shader *shader, const char *vertexPath,
                 const char *fragmentPath) {

  shader->ID = glCreateProgram();

  const char *vertexSource = readShaderFile(vertexPath);
  Shader vertexShader = {glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertexShader.ID, 1, &vertexSource, NULL);
  glCompileShader(vertexShader.ID);
  checkCompileErrors(vertexShader.ID, "VERTEX");

  const char *fragmentSource = readShaderFile(fragmentPath);
  Shader fragmentShader = {glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragmentShader.ID, 1, &fragmentSource, NULL);
  glCompileShader(fragmentShader.ID);
  checkCompileErrors(fragmentShader.ID, "FRAGMENT");

  glAttachShader(shader->ID, vertexShader.ID);
  glAttachShader(shader->ID, fragmentShader.ID);
  glLinkProgram(shader->ID);
  checkCompileErrors(shader->ID, "PROGRAM");

  shader->Locs.model = glGetUniformLocation(shader->ID, "model");
  shader->Locs.view = glGetUniformLocation(shader->ID, "view");
  shader->Locs.projection = glGetUniformLocation(shader->ID, "proj");
  shader->Locs.lightamb = glGetUniformLocation(shader->ID, "light.ambient");
  shader->Locs.lightdiff = glGetUniformLocation(shader->ID, "light.diffuse");
  shader->Locs.lightspec = glGetUniformLocation(shader->ID, "light.specular");
  shader->Locs.lightPos = glGetUniformLocation(shader->ID, "light.position");
  shader->Locs.viewPos = glGetUniformLocation(shader->ID, "viewPos");
  shader->Locs.constant = glGetUniformLocation(shader->ID, "light.constant");
  shader->Locs.linear = glGetUniformLocation(shader->ID, "light.linear");
  shader->Locs.quadratic = glGetUniformLocation(shader->ID, "light.quadratic");

  glDeleteShader(fragmentShader.ID);
  glDeleteShader(vertexShader.ID);
  free((void *)vertexSource);
  free((void *)fragmentSource);
}
void shader_use(Shader *shader) { glUseProgram(shader->ID); }

void shader_set_mat4(Shader *shader, const char *name, float *value) {
  if (strcmp(name, "model") == 0) {
    glUniformMatrix4fv(shader->Locs.model, 1, GL_FALSE, value);
  } else if (strcmp(name, "view") == 0) {
    glUniformMatrix4fv(shader->Locs.view, 1, GL_FALSE, value);
  } else if (strcmp(name, "proj") == 0) {
    glUniformMatrix4fv(shader->Locs.projection, 1, GL_FALSE, value);
  } else if (strcmp(name, "licol") == 0) {
    glUniform3fv(shader->Locs.lightdiff, 1, value);
    glUniform3fv(shader->Locs.lightspec, 1, (float *)(vec3){1.0f, 1.0f, 1.0f});
    glUniform3fv(shader->Locs.lightamb, 1, (float *)(vec3){0.2f, 0.2f, 0.2f});
    glUniform1f(shader->Locs.constant, 1.0f);
    glUniform1f(shader->Locs.linear, 0.22);
    glUniform1f(shader->Locs.quadratic, 0.20);
  } else if (strcmp(name, "lipos") == 0) {
    glUniform3fv(shader->Locs.lightPos, 1, value);
  } else if (strcmp(name, "viewpos") == 0) {
    glUniform3fv(shader->Locs.viewPos, 1, value);
  }
}
void shader_set_int(Shader *shader, char *name, int value) {

  glUniform1f(glGetUniformLocation(shader->ID, name), value);
}
#endif
