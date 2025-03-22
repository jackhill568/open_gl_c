#ifndef MODEL_H
#define MODEL_H
#include "Mesh.h"
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <linmath/linmath.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef struct {
  Mesh *meshes;
  unsigned int numMeshes;
  char *directory;

} Model;

unsigned int TextureFromFile(const char *path, const char *directory) {
  char fullPath[512];
  snprintf(fullPath, sizeof(fullPath), "%s/%s", directory, path);

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(fullPath, &width, &height, &nrChannels, 0);

  if (data) {
    GLenum format;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;
    else
      format = GL_RGB; // Default

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    fprintf(stderr, "Failed to load texture: %s\n", fullPath);
    stbi_image_free(data);
    return 0;
  }

  return textureID;
}

void addElement(void **arr, unsigned int *size, size_t element_size,
                void *newElement) {
  void *temp = realloc(*arr, (*size + 1) * element_size);
  if (temp == NULL) {
    fprintf(stderr, "Memory allocation failed!\n");
    exit(1);
  }
  *arr = temp;
  memcpy((char *)(*arr) + (*size * element_size), newElement, element_size);

  (*size)++;
}

void DrawModel(Shader *shader, Model *model) {
  for (unsigned int i = 0; i < model->numMeshes; i++)
    DrawMesh(shader, &model->meshes[i]);
}

Texture *loadMaterialTextures(struct aiMaterial *mat, enum aiTextureType type, const char *typeName, unsigned int *numTextures, const char *directory) {
  unsigned int textureCount = aiGetMaterialTextureCount(mat, type);
  Texture *textures = (Texture *)malloc(textureCount * sizeof(Texture));
  if (textures == NULL) {
    fprintf(stderr, "Memory allocation for textures failed!\n");
    return NULL;
  }
  for (unsigned int i = 0; i < textureCount; i++) {
    struct aiString str;
    aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

    Texture texture;
    texture.id = TextureFromFile(str.data, directory);
    texture.type = strdup(typeName);
    texture.path = strdup(str.data);
    textures[i] = texture;
  }

  *numTextures = textureCount;
  return textures;
}
Mesh processMesh(struct aiMesh *mesh, const struct aiScene *scene, Model *model) {
  Vertex *vertices;
  unsigned int numVertices;
  unsigned int *indices;
  unsigned int numIndices;
  Texture *textures;
  unsigned int numTextures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.position[0] = mesh->mVertices[i].x;
    vertex.position[1] = mesh->mVertices[i].y;
    vertex.position[2] = mesh->mVertices[i].z;

    vertex.normal[0] = mesh->mNormals[i].x;
    vertex.normal[1] = mesh->mNormals[i].y;
    vertex.normal[2] = mesh->mNormals[i].z;

    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
    {
      vertex.TexCoords[0] = mesh->mTextureCoords[0][i].x;
      vertex.TexCoords[1] = mesh->mTextureCoords[0][i].y;
    } else {
      vertex.TexCoords[0] = 0.0f;
      vertex.TexCoords[1] = 0.0f;
    };

    addElement((void **)vertices, &numVertices, sizeof(Vertex), &vertex);
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++)
      addElement((void **)indices, &numIndices, sizeof(unsigned int), &face.mIndices[j]);
  }
  // process material
  if (mesh->mMaterialIndex >= 0) {
    {
      struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      unsigned int foundTextures;
      Texture *diffuseMaps = loadMaterialTextures(material,
                                                  aiTextureType_DIFFUSE, "texture_diffuse", &foundTextures, model->directory);

      for (int i = 0; i < foundTextures; i++) {
        addElement((void **)textures, &numTextures, sizeof(Texture), &diffuseMaps[i]);
      };
      foundTextures = 0;
      Texture *specularMaps = loadMaterialTextures(material,
                                                   aiTextureType_SPECULAR, "texture_specular", &foundTextures, model->directory);
      for (int i = 0; i < foundTextures; i++) {
        addElement((void **)textures, &numTextures, sizeof(Texture), &specularMaps[i]);
      }
    }
  }

  return (Mesh){vertices, indices, textures};
}
void processNode(struct aiNode *node, const struct aiScene *scene,
                 Model *model) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Mesh newMesh = processMesh(mesh, scene, model);
    addElement((void **)&model->meshes, &model->numMeshes, sizeof(Mesh), &newMesh);
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene, model);
  }
}

void loadModel(char *path, Model *model) {
  const struct aiScene *scene =
      aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene) {
    fprintf(stderr, "Error loading mesh: %s\n", aiGetErrorString());
    return;
  }
  const char *lastSlash = strrchr(path, '/');
  if (lastSlash) {
    size_t dirLength = lastSlash - path;
    char directory[dirLength + 1];
    strncpy(directory, path, dirLength);
    directory[dirLength] = '\0';
  }
  processNode(scene->mRootNode, scene, model);
}

#endif
