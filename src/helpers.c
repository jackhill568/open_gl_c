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
