#ifndef SHAPE_H
#define SHAPE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "linmath/linmath.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
  GLfloat *verticies;
  GLuint *indices;
} Shape;

typedef struct {
  GLuint EBO;
  GLuint VBO;
  GLuint VAO;
  int instanceCount;
} ShapeBuffer;

GLuint cube_indices[] = {
    0, 1, 2, 0, 2, 3, // Back face
    4, 5, 6, 4, 6, 7, // Front face
    3, 2, 6, 3, 6, 7, // Top face
    0, 1, 5, 0, 5, 4, // Bottom face
    0, 3, 7, 0, 7, 4, // Left face
    1, 2, 6, 1, 6, 5  // Right face
};

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
  cube_buffer->VBO = glGenBuffers(1, &cube_buffer->VBO);
  cube_buffer->VAO = glGenVertexArrays(1, &cube_buffer->VAO);
  cube_buffer->EBO = glGenBuffers(1, &cube_buffer->EBO);

  glBindVertexArray(cube_buffer->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, cube_buffer->VBO);
  // glBufferData(GL_ARRAY_BUFFER, 8*6*sizeof(GLfloat), );
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_buffer->EBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        (void *)(3 * sizeof(GLfloat)));
}

void make_shape(Shape *shape, vec3 pos, const char *type, float *colour) {
  if (strcmp(type, "cube") == 0) {

    shape->verticies = (GLfloat *)malloc(6 * 8 * sizeof(GLfloat));
    get_cube_verts(shape->verticies, colour);
    shape->indices = (GLuint *)malloc(36 * sizeof(GLuint));
    memcpy(shape->indices, cube_indices, sizeof(cube_indices));

    glBufferData(GL_ARRAY_BUFFER, 8 * 6 * sizeof(GLfloat), shape->verticies,
                 GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shape->indices),
                 shape->indices, GL_STATIC_DRAW);
  }
}

GLuint vao, vbo, EBO;
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);

glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// Position attribute
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);

// Texture coord attribute
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                      (void *)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
