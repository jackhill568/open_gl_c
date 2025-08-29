#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
  unsigned int id;
  char *type;
  char *path;
} Texture;

unsigned int TextureFromFile(const char *path, const char *directory);

#endif
