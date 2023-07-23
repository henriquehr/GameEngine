#pragma once

#include "game_object.h"
#include "window.h"

class KeyboardMovementController {
  public:
    struct KeyMappings {
        int moveLeft = SDL_SCANCODE_A;
        int moveRight = SDL_SCANCODE_D;
        int moveForward = SDL_SCANCODE_W;
        int moveBackward = SDL_SCANCODE_S;
        int moveUp = SDL_SCANCODE_E;
        int moveDown = SDL_SCANCODE_Q;
        int lookLeft = SDL_SCANCODE_LEFT;
        int lookRight = SDL_SCANCODE_RIGHT;
        int lookUp = SDL_SCANCODE_UP;
        int lookDown = SDL_SCANCODE_DOWN;
    };

    KeyMappings keys{};
    float moveSpeed{5.0f};
    float lookSpeed{1.5f};

    void moveInPlaneXZ(float deltaTime, GameObject &gameObject);
};
