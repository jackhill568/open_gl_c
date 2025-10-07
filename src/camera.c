#include "camera.h"
#include "Mesh.h"
#include <linmath/linmath.h>
#include <stdbool.h>
void camera_get_view_matrix(Camera *camera, mat4x4 view) {
  vec3 target;
  vec3_add(target, camera->position, camera->front);

  vec3 zaxis;
  vec3_sub(zaxis, camera->position, target);
  vec3_norm(zaxis, zaxis);

  vec3 xaxis;
  vec3_mul_cross(xaxis, camera->worldUp, zaxis);
  vec3_norm(xaxis, xaxis);

  vec3 yaxis;
  vec3_mul_cross(yaxis, zaxis, xaxis);

  view[0][0] = xaxis[0];
  view[0][1] = yaxis[0];
  view[0][2] = zaxis[0];
  view[0][3] = 0.0f;
  view[1][0] = xaxis[1];
  view[1][1] = yaxis[1];
  view[1][2] = zaxis[1];
  view[1][3] = 0.0f;
  view[2][0] = xaxis[2];
  view[2][1] = yaxis[2];
  view[2][2] = zaxis[2];
  view[2][3] = 0.0f;
  view[3][0] = -vec3_mul_inner(xaxis, camera->position);
  view[3][1] = -vec3_mul_inner(yaxis, camera->position);
  view[3][2] = -vec3_mul_inner(zaxis, camera->position);
  view[3][3] = 1.0f;
}
void camera_update_vectors(Camera *camera) {
  vec3 front;
  front[0] =
      cos(camera->yaw * M_PI / 180.0f) * cos(camera->pitch * M_PI / 180.0f);
  front[1] = sin(camera->pitch * M_PI / 180.0f);
  front[2] =
      sin(camera->yaw * M_PI / 180.0f) * cos(camera->pitch * M_PI / 180.0f);
  vec3_norm(camera->front, front);

  vec3_mul_cross(camera->right, camera->front, camera->worldUp);
  vec3_norm(camera->right, camera->right);
  vec3_mul_cross(camera->up, camera->right, camera->front);
  vec3_norm(camera->up, camera->up);
}
void camera_init(Camera *camera) {

  vec3 position = {0.0f, 100.0f, 30.0f};
  vec3 worldUp = {0.0f, 1.0f, 0.0f};
  vec3_set(camera->front, (vec3){1, 1, 0});
  vec3_dup(camera->position, position);
  vec3_dup(camera->worldUp, worldUp);
  camera->yaw = -90.0f;
  camera->pitch = 0.0f;
  camera->movementSpeed = 80.0f;
  camera->mouseSensitivity = 0.04f;
  camera->zoom = 45.0f;
  camera_update_vectors(camera);
}

void camera_process_mouse(Camera *camera, float xoffset, float yoffset,
                          bool constrainPitch) {
  xoffset *= camera->mouseSensitivity;
  yoffset *= camera->mouseSensitivity;

  camera->yaw += xoffset;
  camera->pitch += yoffset;

  if (constrainPitch) {
    if (camera->pitch > 89.0f)
      camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
      camera->pitch = -89.0f;
  }

  camera_update_vectors(camera);
}

void camera_process_keyboard(Camera *camera, int direction, float deltaTime) {
  float velocity = camera->movementSpeed * deltaTime;
  vec3 temp;

  switch (direction) {
  case GLFW_KEY_W:
    vec3_scale(temp, camera->front, velocity);
    vec3_add(camera->position, camera->position, temp);
    break;
  case GLFW_KEY_S:
    vec3_scale(temp, camera->front, velocity);
    vec3_sub(camera->position, camera->position, temp);
    break;
  case GLFW_KEY_A:
    vec3_scale(temp, camera->right, velocity);
    vec3_sub(camera->position, camera->position, temp);
    break;
  case GLFW_KEY_D:
    vec3_scale(temp, camera->right, velocity);
    vec3_add(camera->position, camera->position, temp);
    break;
  }
}
