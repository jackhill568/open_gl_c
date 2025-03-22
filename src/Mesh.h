#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include <linmath/linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  vec3 position;
  vec3 normal;
  vec2 TexCoords;
} Vertex;

typedef struct {
  unsigned int id;
  char *type;
  char *path;
} Texture;

typedef struct {
  Vertex *vertices;
  unsigned int *indices;
  Texture *textures;
  unsigned int VAO, VBO, EBO;
  unsigned int numVertices, numIndices, numTextures;
} Mesh;

Mesh create_Mesh(Mesh mesh, Vertex *vertices, unsigned int *indices,
                 Texture *textures) {
  mesh.vertices = vertices;
  mesh.indices = indices;
  mesh.textures = textures;
  return mesh;
};

void setupMesh(Mesh *mesh) {
  glGenVertexArrays(1, &mesh->VAO);
  glGenBuffers(1, &mesh->VBO);
  glGenBuffers(1, &mesh->EBO);

  glBindVertexArray(mesh->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->numVertices,
               &mesh->vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->numIndices,
               &mesh->indices[0], GL_STATIC_DRAW);

  // vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  // vertex normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, normal));
  // vertex texture coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));

  glBindVertexArray(0);
};
void DrawMesh(Shader *shader, Mesh *mesh) {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < mesh->numTextures; i++) {
    glActiveTexture(GL_TEXTURE0 +
                    i); // activate proper texture unit before binding
    // retrieve texture number (the N in diffuse_textureN)
    char number[3];
    char name[17];
    strcpy(name, mesh->textures[i].type);

    if (strcmp(name, "texture_diffuse") == 0) {
      sprintf(number, "%d", diffuseNr++);
    } else if (strcmp(name, "texture_specular") == 0) {
      sprintf(number, "%d", specularNr++);
    }
    char shaderName[28];
    strcpy(shaderName, "material.");
    strcpy(shaderName, name);
    strcpy(shaderName, number);
    shader_set_int(shaderName, i);
    glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
  }
  glActiveTexture(GL_TEXTURE0);

  // draw mesh
  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

#endif // !MESH_H
