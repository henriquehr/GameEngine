
#include "keyboard_movement_controller.h"

void KeyboardMovementController::moveInPlaneXZ(float deltaTime, GameObject &gameObject) {
    glm::vec3 rotate{0};
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);
    if (currentKeyStates[keys.lookRight]) {
        rotate.y += 1.0f;
    }
    if (currentKeyStates[keys.lookLeft]) {
        rotate.y -= 1.0f;
    }
    if (currentKeyStates[keys.lookUp]) {
        rotate.x += 1.0f;
    }
    if (currentKeyStates[keys.lookDown]) {
        rotate.x -= 1.0f;
    }

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
        gameObject.transform.rotation += lookSpeed * deltaTime * glm::normalize(rotate);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{glm::sin(yaw), 0.0f, glm::cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

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
