#include "Model.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include "stb_image.h"
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <linmath/linmath.h>
#include <stdio.h>
#include <stdlib.h>

void DrawModel(Shader *shader, Model *model) {
  if (shader == NULL || model == NULL) {
    fprintf(stderr, "Invalid shader or model in DrawModel\n");
    return;
  }

  struct Node *temp = model->meshes;
  while (temp != NULL) {
    Mesh *mesh = (Mesh *)temp->data;
    if (mesh != NULL) {
      // Skip meshes with invalid VAO
      if (mesh->VAO == 0) {
        fprintf(stderr, "Warning: Mesh with VAO=0 found, skipping draw\n");
        temp = temp->next;
        continue;
      }

      // Draw the mesh
      DrawMesh(shader, mesh);
    }
    temp = temp->next;
  }
}

Texture *loadMaterialTextures(struct aiMaterial *mat, enum aiTextureType type, unsigned int *numTextures, const char *typeName, const char *directory, Model *model) {
  unsigned int textureCount = aiGetMaterialTextureCount(mat, type);
  Texture *textures = (Texture *)malloc(textureCount * sizeof(Texture));
  int countTextures = 0;

  printf("num of textures %d \n", textureCount);

  if (textures == NULL) {
    fprintf(stderr, "Memory allocation for textures failed!\n");
    return NULL;
  }
  for (unsigned int i = 0; i < textureCount; i++) {
    struct aiString str;
    aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);
    int skip = 0;
    for (unsigned int j = 0; j < countTextures; j++) {
      if (strcmp(textures[j].path, str.data) == 0) {
        textures[i] = textures[j];
        countTextures++;
        skip = 1;
        break;
      }
    }
    if (skip != 1) {
      Texture texture;
      texture.id = TextureFromFile(str.data, directory);
      texture.type = strdup(typeName);
      if (texture.type == NULL) {
        fprintf(stderr, "Memory allocation for texture type failed!\n");
        // Free previously allocated textures
        for (unsigned int k = 0; k < i; k++) {
          if (textures[k].type != NULL && k != i)
            free(textures[k].type);
          if (textures[k].path != NULL && k != i)
            free(textures[k].path);
        }
        free(textures);
        return NULL;
      }

      texture.path = strdup(str.data);
      if (texture.path == NULL) {
        fprintf(stderr, "Memory allocation for texture path failed!\n");
        free(texture.type);
        // Free previously allocated textures
        for (unsigned int k = 0; k < i; k++) {
          if (textures[k].type != NULL)
            free(textures[k].type);
          if (textures[k].path != NULL)
            free(textures[k].path);
        }
        free(textures);
        return NULL;
      }
    }
  }
  *numTextures = textureCount;
  return textures;
}

Mesh processMesh(struct aiMesh *mesh, const struct aiScene *scene, Model *model) {
  struct Node *textures;

  Mesh newMesh;
  newMesh.numVertices = 0;
  newMesh.VAO = 0;
  newMesh.VBO = 0;
  newMesh.EBO = 0;

  Vertex *vertices = (Vertex *)malloc(mesh->mNumVertices * sizeof(Vertex));
  if (vertices == NULL) {
    fprintf(stderr, "Memory allocation for vertices failed!\n");
    newMesh.vertices = NULL;
    newMesh.indices = NULL;
    return newMesh;
  }
  unsigned int totalIndices = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    totalIndices += mesh->mFaces[i].mNumIndices;
  }
  newMesh.numIndices = totalIndices;

  // Allocate memory for indices
  unsigned int *indices = (unsigned int *)malloc(totalIndices * sizeof(unsigned int));
  if (indices == NULL) {
    fprintf(stderr, "Memory allocation for indices failed!\n");
    free(vertices);
    newMesh.vertices = NULL;
    newMesh.indices = NULL;
    return newMesh;
  }

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
    vertices[i] = vertex;
    newMesh.numVertices++;
  }

  unsigned int indexCount = 0;
  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    struct aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices[indexCount++] = face.mIndices[j];
    }
  }

  newMesh.vertices = vertices;
  newMesh.indices = indices;
  // process material
  //
  unsigned int texturesFound = 0;
  if (mesh->mMaterialIndex >= 0) {
    {
      struct aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
      Texture *diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, &texturesFound, "texture_diffuse", model->directory, model);

      if (diffuseMaps != NULL && texturesFound > 0) {
        for (unsigned int i = 0; i < texturesFound; i++) {
          append(&newMesh.textures, &diffuseMaps[i], sizeof(Texture));
        }
        free(diffuseMaps);
      }
      texturesFound = 0;
      Texture *specularMaps = loadMaterialTextures(material,
                                                   aiTextureType_SPECULAR, &texturesFound, "texture_specular", model->directory, model);
      if (specularMaps != NULL && texturesFound > 0) {
        for (unsigned int i = 0; i < texturesFound; i++) {
          append(&newMesh.textures, &specularMaps[i], sizeof(Texture));
        }
        free(specularMaps); // Free the array after copying data to linked list
      }

      // shiny-ness
      //if (aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &newMesh.shininess) != AI_SUCCESS) {
       // newMesh.shininess = 32.0f;
      //}
    }
  }
  return newMesh;
}
void processNode(struct aiNode *node, const struct aiScene *scene,
                 Model *model) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    struct aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    Mesh newMesh = processMesh(mesh, scene, model);
    if (newMesh.vertices != NULL && newMesh.indices != NULL) {
      append(&model->meshes, &newMesh, sizeof(Mesh));
    }
  }
  // then do the same for each of its children
  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene, model);
  }
}

void loadModel(char *path, Model *model) {
  model->meshes = NULL;

  const struct aiScene *scene = aiImportFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
  if (!scene) {
    fprintf(stderr, "Error loading mesh: %s\n", aiGetErrorString());
    return;
  }

  // Extract directory from path
  const char *lastSlash = strrchr(path, '/');
  if (lastSlash) {
    size_t dirLength = lastSlash - path;
    model->directory = malloc(dirLength + 1);
    if (model->directory == NULL) {
      fprintf(stderr, "Memory allocation for directory failed!\n");
      aiReleaseImport(scene);
      return;
    }
    strncpy(model->directory, path, dirLength);
    model->directory[dirLength] = '\0';
  } else {
    model->directory = strdup(".");
    if (model->directory == NULL) {
      fprintf(stderr, "Memory allocation for directory failed!\n");
      aiReleaseImport(scene);
      return;
    }
  }

  processNode(scene->mRootNode, scene, model);

  struct Node *current = model->meshes;
  while (current != NULL) {
    Mesh *mesh = (Mesh *)current->data;
    if (mesh != NULL) {
      // Initialize VAO, VBO, EBO to 0
      mesh->VAO = 0;
      mesh->VBO = 0;
      mesh->EBO = 0;
      // Setup the mesh
      setupMesh(mesh);

      // Debug output to confirm mesh setup
      printf("Mesh setup complete: VAO=%u, vertices=%u, indices=%u \n",
             mesh->VAO, mesh->numVertices, mesh->numIndices);
    }
    current = current->next;
  }

  aiReleaseImport(scene);
}
void check_gl_error(const char *label) {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    printf("OpenGL error at %s: 0x%04x\n", label, err);
  }
}
void clean_model(Model *model) {
  struct Node *current = model->meshes;
  while (current != NULL) {
    struct Node *next = current->next;
    clean_mesh((Mesh *)current->data);
    free(current->data);
    free(current);
    current = next;
  }

  free(model->directory);
  model->meshes = NULL;
  model->directory = NULL;
}
