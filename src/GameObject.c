#include "GameObject.h"
#include <GLFW/glfw3.h>
#include "helpers.h"
#include "linmath/linmath.h"
#include "shader.h"
#include "stdbool.h"

void draw_obj(GameObject *obj, Shader *shader) {
  mat4x4 model;
  mat4x4_identity(model);

  mat4x4_rotate_Y(model, model, (-obj->yaw + 90) * M_PI / 180.0f);
  mat4x4_rotate_X(model, model, -obj->pitch * M_PI / 180.0f);

  mat4x4 translation;
  mat4x4_identity(translation);
  mat4x4_translate(translation, obj->position[0], obj->position[1], obj->position[2]);

  mat4x4 finalModel;
  mat4x4_mul(finalModel, translation, model);

  shader_use(shader);
  shader_set_mat4(shader, "model", (float *)finalModel);
  DrawModel(shader, &obj->sprite);
}

void obj_process_mouse(GameObject *obj, float xoffset, float yoffset,
                       bool constrainPitch) {
  xoffset *= 0.1;
  yoffset *= 0.1;

  obj->yaw += xoffset;
  obj->pitch += yoffset;

  if (constrainPitch) {
    if (obj->pitch > 89.0f)
      obj->pitch = 89.0f;
    if (obj->pitch < -89.0f)
      obj->pitch = -89.0f;
  }

  obj_update_vectors(obj);
}
void obj_update_vectors(GameObject *obj) {
  vec3 front;
  front[0] =
      cos(obj->yaw * M_PI / 180.0f) * cos(obj->pitch * M_PI / 180.0f);
  front[1] = sin(obj->pitch * M_PI / 180.0f);
  front[2] =
      sin(obj->yaw * M_PI / 180.0f) * cos(obj->pitch * M_PI / 180.0f);
  vec3_norm(obj->front, front);

  vec3_mul_cross(obj->right, obj->front, (vec3){0, 1, 0});
  vec3_norm(obj->right, obj->right);
  vec3_mul_cross(obj->up, obj->right, obj->front);
  vec3_norm(obj->up, obj->up);
}
void obj_process_keyboard(GameObject *obj, int direction, float deltaTime) {
  float velocity = 40 * deltaTime;
  vec3 temp;

  switch (direction) {
  case GLFW_KEY_W:
    vec3_scale(temp, obj->front, velocity);
    vec3_add(obj->position, obj->position, temp);
    break;
  case GLFW_KEY_S:
    vec3_scale(temp, obj->front, velocity);
    vec3_sub(obj->position, obj->position, temp);
    break;
  case GLFW_KEY_A:
    vec3_scale(temp, obj->right, velocity);
    vec3_sub(obj->position, obj->position, temp);
    break;
  case GLFW_KEY_D:
    vec3_scale(temp, obj->right, velocity);
    vec3_add(obj->position, obj->position, temp);
    break;
  case GLFW_KEY_SPACE:
    break;
  }
}
