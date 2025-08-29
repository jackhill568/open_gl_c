#include "helpers.h"
#include <linmath/linmath.h>
#include <stdlib.h>

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
void mat4x4_translate_vec3(mat4x4 M, vec3 v) {
  mat4x4_translate(M, v[0], v[1], v[2]);
}
void vec2_set(float *M, float *T) {

  M[0] = T[0];
  M[1] = T[1];
}
void vec3_set(float *M, float *T) {

  M[0] = T[0];
  M[1] = T[1];
  M[2] = T[2];
}
