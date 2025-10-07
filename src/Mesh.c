#include "Mesh.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "helpers.h"
#include "shader.h"
#include <linmath/linmath.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  if (mesh == NULL || shader == NULL) {
    fprintf(stderr, "Invalid mesh or shader in DrawMesh\n");
    return;
  }

  // Check if VAO is valid
  if (mesh->VAO == 0) {
    fprintf(stderr, "Mesh not properly set up. VAO is 0.\n");
    return;
  }

  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;

  // Process textures if they exist
  if (mesh->textures != NULL) {
    struct Node *temp = mesh->textures;
    int i = 0;

    while (temp != NULL && temp->data != NULL) {
      Texture *texture = (Texture *)temp->data;

      // Validate texture data
      if (texture->type == NULL) {
        // fprintf(stderr, "Texture type is NULL\n");
        temp = temp->next;
        continue;
      }

      glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit

      // Create texture name for shader
      char number[16] = {0};
      char name[32] = {0};
      strncpy(name, texture->type, sizeof(name) - 1);

      if (strcmp(name, "texture_diffuse") == 0) {
        snprintf(number, sizeof(number), "%d", diffuseNr++);
      } else if (strcmp(name, "texture_specular") == 0) {
        snprintf(number, sizeof(number), "%d", specularNr++);
      }

      char shaderName[64] = "material.";
      strncat(shaderName, name, sizeof(shaderName) - strlen(shaderName) - 1);
      strncat(shaderName, number, sizeof(shaderName) - strlen(shaderName) - 1);

      // Debug output
      printf("Setting shader uniform: %s = %d\n", shaderName, i);

      shader_set_int(shader, shaderName, i);

      // Check for valid texture ID
      if (texture->id > 0) {
        glBindTexture(GL_TEXTURE_2D, texture->id);
      } else {
        fprintf(stderr, "Invalid texture ID: %u\n", texture->id);
      }

      i++;
      temp = temp->next;
    }
  }

  shader_set_float(shader, "material.shininess", mesh->shininess);

  glActiveTexture(GL_TEXTURE0);

  // Draw mesh
  glBindVertexArray(mesh->VAO);
  glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void clean_mesh(Mesh *mesh) {
  if (mesh == NULL) {
    return;
  }

  // Clean up OpenGL resources
  if (mesh->VAO != 0) {
    glDeleteVertexArrays(1, &mesh->VAO);
    mesh->VAO = 0;
  }
  if (mesh->VBO != 0) {
    glDeleteBuffers(1, &mesh->VBO);
    mesh->VBO = 0;
  }
  if (mesh->EBO != 0) {
    glDeleteBuffers(1, &mesh->EBO);
    mesh->EBO = 0;
  }

  // Free texture resources
  struct Node *current = mesh->textures;
  struct Node *next = NULL;

  while (current != NULL) {
    next = current->next;

    Texture *texture = (Texture *)current->data;

    free(current->data);
    free(current);
    current = next;
  }

  // Free vertex and index data
  if (mesh->vertices != NULL) {
    free(mesh->vertices);
    mesh->vertices = NULL;
  }

  if (mesh->indices != NULL) {
    free(mesh->indices);
    mesh->indices = NULL;
  }

  mesh->numVertices = 0;
  mesh->numIndices = 0;
  mesh->textures = NULL;
}
Mesh create_Mesh(Mesh mesh, Vertex *vertices, unsigned int *indices,
                 Texture *textures) {

  mesh.vertices = vertices;
  mesh.indices = indices;
  append(&mesh.textures, textures, sizeof(Texture));
  return mesh;
};
