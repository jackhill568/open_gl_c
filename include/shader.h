
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

void checkCompileErrors(GLuint shader, char *type);

char *readShaderFile(const char *filename);

void shader_init(Shader *shader, const char *vertexPath,
                 const char *fragmentPath);

void shader_use(Shader *shader);

void shader_set_mat4(Shader *shader, const char *name, float *value);

void shader_set_int(Shader *shader, char *name, int value);

#endif
