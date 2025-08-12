#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <linmath/linmath.h>
#include <stdbool.h>

typedef struct {
  vec3 position;
  vec3 front;
  vec3 up;
  vec3 right;
  vec3 worldUp;
  float yaw;
  float pitch;
  float movementSpeed;
  float mouseSensitivity;
  float zoom;
} Camera;

void camera_update_vectors(Camera *camera);

void camera_get_view_matrix(Camera *camera, mat4x4 view);

void camera_process_mouse(Camera *camera, float xoffset, float yoffset,
                          bool constrainPitch);

void camera_init(Camera *camera);
void camera_process_keyboard(Camera *camera, int direction, float deltaTime);

#endif
