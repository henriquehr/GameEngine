
#include "first_person_movement_controller.h"

FirstPersonMovementController::FirstPersonMovementController(glm::vec3 position) : position(position) {
    updateCameraVectors();
}

void FirstPersonMovementController::processKeyPress(float deltaTime) {
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);

    float velocity = moveSpeed * deltaTime;

    glm::vec3 moveDir{0.0f};
    if (currentKeyStates[keys.moveForward]) {
        position += front * velocity;
    }
    if (currentKeyStates[keys.moveBackward]) {
        position -= front * velocity;
    }
    if (currentKeyStates[keys.moveRight]) {
        position += glm::normalize(glm::cross(front, up)) * velocity;
    }
    if (currentKeyStates[keys.moveLeft]) {
        position -= glm::normalize(glm::cross(front, up)) * velocity;
    }
    if (currentKeyStates[keys.moveUp]) {
        position += up * velocity;
    }
    if (currentKeyStates[keys.moveDown]) {
        position -= up * velocity;
    }
}

void FirstPersonMovementController::processMouseMovement(float yOffset, float xOffset) {
    yaw -= xOffset * lookSpeed;
    pitch += yOffset * lookSpeed;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

void FirstPersonMovementController::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
}