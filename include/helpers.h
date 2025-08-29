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

void mat4x4_translate_vec3(mat4x4 M, vec3 v);
void vec2_set(float *M, float *T);
void vec3_set(float *M, float *T);
#endif // !HELPERS_H
#define HELPERS_H
