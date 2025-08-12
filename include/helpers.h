#ifndef HELPERS_H
#define HELPERS_H
#include <linmath/linmath.h>
#include <stdlib.h>
struct Node {
  void *data;
  struct Node *next;
};

void append(struct Node **head, void *newData, size_t dataSize);

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

#endif // !HELPERS_H
#define HELPERS_H
