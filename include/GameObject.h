#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Model.h"
#include "linmath/linmath.h"
typedef struct {

  Model *sprite;

  vec3 position;
  vec2 rotation;

} GameObject;

#endif
