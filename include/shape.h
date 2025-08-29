#ifndef SHAPE_H
#define SHAPE_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "linmath/linmath.h"
#include "shader.h"

#include "shader.h"

typedef struct {
  GLfloat *vertices;
  GLuint *indices;
  int index_count;
  vec3 pos;
} Shape;

typedef struct {
  GLuint EBO;
  GLuint VBO;
  GLuint VAO;
  int instanceCount;
} ShapeBuffer;

void get_cube_indices(GLuint *indices);
void get_cube_verts(GLfloat *vertices, float *colour);

void init_shapes(ShapeBuffer *cube_buffer);
void clean_buffers(ShapeBuffer *shape_buffer);
void make_shape(Shape *shape, vec3 pos, const char *type, float *colour);
void draw_shape(Shape *shape, ShapeBuffer *cube_buffer, Shader *shader);

#endif
