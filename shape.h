#ifndef SHAPE_H
#define SHAPE_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "linmath/linmath.h"
#include "shader.h"
#include <stdlib.h>
#include <string.h>

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

void get_cube_indices(GLuint *indices) {
  GLuint cube_indices[] = {
      0, 1, 2, 0, 2, 3, // Back face
      4, 5, 6, 4, 6, 7, // Front face
      3, 2, 6, 3, 6, 7, // Top face
      0, 1, 5, 0, 5, 4, // Bottom face
      0, 3, 7, 0, 7, 4, // Left face
      1, 2, 6, 1, 6, 5  // Right face
  };
  memcpy(indices, cube_indices, sizeof(cube_indices));
}

void get_cube_verts(GLfloat *vertices, float *colour) {
  GLfloat cube_vertices[] = {
      // Positions         // Colors
      -0.5f, -0.5f, -0.5f, colour[0], colour[1], colour[2],
      0.5f,  -0.5f, -0.5f, colour[0], colour[1], colour[2],
      0.5f,  0.5f,  -0.5f, colour[0], colour[1], colour[2],
      -0.5f, 0.5f,  -0.5f, colour[0], colour[1], colour[2],
      -0.5f, -0.5f, 0.5f,  colour[0], colour[1], colour[2],
      0.5f,  -0.5f, 0.5f,  colour[0], colour[1], colour[2],
      0.5f,  0.5f,  0.5f,  colour[0], colour[1], colour[2],
      -0.5f, 0.5f,  0.5f,  colour[0], colour[1], colour[2]};
  memcpy(vertices, cube_vertices, sizeof(cube_vertices));
}

void init_shapes(ShapeBuffer *cube_buffer) {
  // Generate buffers correctly
  glGenBuffers(1, &cube_buffer->VBO);
  glGenVertexArrays(1, &cube_buffer->VAO);
  glGenBuffers(1, &cube_buffer->EBO);

  glBindVertexArray(cube_buffer->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, cube_buffer->VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_buffer->EBO);

  // Enable vertex attributes
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
}

void make_shape(Shape *shape, vec3 pos, const char *type, float *colour) {
  if (strcmp(type, "cube") == 0) {
    shape->vertices = (GLfloat *)malloc(6 * 8 * sizeof(GLfloat));
    get_cube_verts(shape->vertices, colour);

    shape->indices = (GLuint *)malloc(36 * sizeof(GLuint));
    get_cube_indices(shape->indices);
    shape->index_count = 36;

    vec3_dup(shape->pos, pos);

    glBufferData(GL_ARRAY_BUFFER, 8 * 6 * sizeof(GLfloat), shape->vertices,
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), shape->indices,
                 GL_STATIC_DRAW);
  }
}

void draw_cube(Shape *shape, ShapeBuffer *cube_buffer, Shader *shader) {

  mat4x4 model;
  mat4x4_translate(model, shape->pos[0], shape->pos[1], shape->pos[2]);

  shader_set_mat4(shader, "model", (float *)model);

  glBindVertexArray(cube_buffer->VAO);
  glDrawElements(GL_TRIANGLES, shape->index_count, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void clean_buffers(ShapeBuffer *shape_buffer) {
  glDeleteVertexArrays(1, &shape_buffer->VAO);
  glDeleteBuffers(1, &shape_buffer->VBO);
  glDeleteBuffers(1, &shape_buffer->EBO);
}

#endif
