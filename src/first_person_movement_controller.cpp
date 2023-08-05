
#include "first_person_movement_controller.h"

glm::mat4 FirstPersonMovementController::updateRotationMatrix() {
    glm::mat4 yaw_rot = glm::rotate(glm::mat4{1}, -yaw, {0, -1, 0});
    rotationMatrix = glm::rotate(glm::mat4{yaw_rot}, pitch, {-1, 0, 0});
    return rotationMatrix;
}

void FirstPersonMovementController::move(float deltaTime, GameObject &gameObject) {
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);

    glm::mat4 camRot = updateRotationMatrix();
    glm::vec3 forwardDir = camRot * glm::vec4(0, 0, 1, 0.f);
    glm::vec3 rightDir = camRot * glm::vec4(1, 0, 0, 0.f);
    glm::vec3 upDir = {0, 1, 0};

    glm::vec3 moveDir{0.0f};
    if (currentKeyStates[keys.moveForward]) {
        moveDir += forwardDir;
    }
    if (currentKeyStates[keys.moveBackward]) {
        moveDir -= forwardDir;
    }
    if (currentKeyStates[keys.moveRight]) {
        moveDir += rightDir;
    }
    if (currentKeyStates[keys.moveLeft]) {
        moveDir -= rightDir;
    }
    if (currentKeyStates[keys.moveUp]) {
        moveDir += upDir;
    }
    if (currentKeyStates[keys.moveDown]) {
        moveDir -= upDir;
    }
    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.translation += moveSpeed * deltaTime * glm::normalize(moveDir);
    }
}
