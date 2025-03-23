#ifndef HELPERS_H
#define HELPERS_H
#include <linmath/linmath.h>
#include <stdlib.h>
struct Node {
  void *data;
  struct Node *next;
};

void append(struct Node **head, void *newData, size_t dataSize) {
  struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));

  newNode->data = malloc(dataSize);
  memcpy(newNode->data, newData, dataSize);

  newNode->next = NULL;
  if (*head == NULL) {
    *head = newNode;
    return;
  }
  struct Node *temp = *head;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = newNode;
}
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
  struct Node *textures;
  unsigned int VAO, VBO, EBO;
  unsigned int numVertices, numIndices, numTextures;
} Mesh;

typedef struct {
  struct Node *meshes;
  char *directory;
} Model;

#endif // !HELPERS_H
#define HELPERS_H
