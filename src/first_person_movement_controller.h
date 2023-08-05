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

    FirstPersonMovementController(glm::vec3 position);

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f);

    void processKeyPress(float deltaTime);

    void processMouseMovement(float xOffset, float yOffset);

  private:
    KeyMappings keys{};
    float moveSpeed{5.0f};
    float lookSpeed{0.5f};
    float pitch = 0;// up-down rotation
    float yaw = 90; // left-right rotation

    void updateCameraVectors();
};
