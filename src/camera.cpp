
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"

void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
    projectionMatrix = glm::ortho(left, right, bottom, top, near, far);
    projectionMatrix[2][2] = -projectionMatrix[2][2];
    projectionMatrix[2][3] = -projectionMatrix[2][3];
    //    projectionMatrix[1][1] *= -1;
}

void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
    projectionMatrix[2][2] = -projectionMatrix[2][2];
    projectionMatrix[2][3] = -projectionMatrix[2][3];
    //    projectionMatrix[1][1] *= -1;
}

void Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    viewMatrix = glm::lookAt(position, position + direction, up);
    inverseViewMatrix = glm::inverse(viewMatrix);
}

void Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
    setViewDirection(position, target - position, up);
}

void Camera::setView(glm::vec3 position, glm::mat4 rotation) {
    viewMatrix = glm::inverse(glm::translate(glm::mat4{1}, position) * rotation);
}
