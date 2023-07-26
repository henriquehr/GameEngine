#pragma once

#include "game_object.h"
#include "window.h"

class FirstPersonMovementController {
  public:
    struct KeyMappings {
        int moveLeft = SDL_SCANCODE_A;
        int moveRight = SDL_SCANCODE_D;
        int moveForward = SDL_SCANCODE_W;
        int moveBackward = SDL_SCANCODE_S;
        int moveUp = SDL_SCANCODE_E;
        int moveDown = SDL_SCANCODE_Q;
    };

    KeyMappings keys{};
    float moveSpeed{5.0f};
    float lookSpeed{0.005f};
    float pitch = 0;
    float yaw = 0;
    glm::mat4 mouseRotation;

    void setPitchYaw(int pitch, int yaw) {
        this->pitch += pitch * lookSpeed;
        this->yaw -= yaw * lookSpeed;
    }
    glm::mat4 getMouseRotation() {
        return mouseRotation;
    }

    glm::mat4 updateMouseRotation();

    void move(float deltaTime, GameObject &gameObject);
};
