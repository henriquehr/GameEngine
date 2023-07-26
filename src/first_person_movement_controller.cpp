
#include "first_person_movement_controller.h"

glm::mat4 FirstPersonMovementController::updateMouseRotation() {
    glm::mat4 yaw_rot = glm::rotate(glm::mat4{1}, yaw, {0, -1, 0});
    mouseRotation = glm::rotate(glm::mat4{yaw_rot}, pitch, {-1, 0, 0});
    return mouseRotation;
}

void FirstPersonMovementController::move(float deltaTime, GameObject &gameObject) {
    glm::vec3 rotate{0};
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);

    float yaw = gameObject.transform.rotation.y;
    glm::vec3 forwardDir{glm::sin(yaw), 0.0f, glm::cos(yaw)};
    glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

    glm::mat4 camRot = updateMouseRotation();
    forwardDir = camRot * glm::vec4(forwardDir, 0.f);
    rightDir = camRot * glm::vec4(rightDir, 0.f);

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
