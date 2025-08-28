#ifndef MODEL_H
#define MODEL_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <linmath/linmath.h>
#define STB_IMAGE_IMPLEMENTATION

typedef struct {
  struct Node *meshes;
  char *directory;
} Model;

void DrawModel(Shader *shader, Model *model);

Texture *loadMaterialTextures(struct aiMaterial *mat, enum aiTextureType type, unsigned int *numTextures, const char *typeName, const char *directory, Model *model);

Mesh processMesh(struct aiMesh *mesh, const struct aiScene *scene, Model *model);

void processNode(struct aiNode *node, const struct aiScene *scene,
                 Model *model);

void loadModel(char *path, Model *model);

void check_gl_error(const char *label);

void clean_model(Model *model);

#endif
