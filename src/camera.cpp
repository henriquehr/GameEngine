
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"

void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
    assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
    projectionMatrix = glm::perspective(glm::radians(fovy), aspect, near, far);
    projectionMatrix[1][1] *= -1;
}

void Camera::setView(glm::vec3 position, glm::vec3 front, glm::vec3 up) {
    viewMatrix = glm::lookAt(position, position + front, up);
}
