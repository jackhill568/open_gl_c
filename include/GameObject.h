#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"
#include "camera.h"
#include "linmath/linmath.h"
#include "shader.h"
#include <stdbool.h>
typedef struct {

  Model sprite;

  vec3 position;
  vec3 front;
  vec3 right;
  vec3 up;
  float yaw;
  float pitch;
  float scale;

} GameObject;

void draw_obj(GameObject *obj, Shader *shader);

void obj_update_vectors(GameObject *obj);
void obj_process_keyboard(GameObject *obj, int direction, float deltaTime);
void obj_process_mouse(GameObject *obj, float xoffset, float yoffset, bool constrainPitch);
void obj_sync_with_camera(GameObject *obj, Camera *cam);
#endif
