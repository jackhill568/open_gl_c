#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Texture.h"
#include "helpers.h"
#include "shader.h"
#include <linmath/linmath.h>
#include <stddef.h>

typedef struct {
  Vertex *vertices;
  unsigned int *indices;
  struct Node *textures;
  unsigned int VAO, VBO, EBO;
  float shininess;
  unsigned int numVertices, numIndices, numTextures;
} Mesh;

void setupMesh(Mesh *mesh);

void DrawMesh(Shader *shader, Mesh *mesh);

void clean_mesh(Mesh *mesh);

Mesh create_Mesh(Mesh mesh, Vertex *vertices, unsigned int *indices,
                 Texture *textures);

#endif // !MESH_H
