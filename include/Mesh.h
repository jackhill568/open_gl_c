#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "helpers.h"
#include "shader.h"
#include <linmath/linmath.h>
#include <stddef.h>

void setupMesh(Mesh *mesh);

void DrawMesh(Shader *shader, Mesh *mesh);

void clean_mesh(Mesh *mesh);

Mesh create_Mesh(Mesh mesh, Vertex *vertices, unsigned int *indices,
                 Texture *textures);

#endif // !MESH_H
