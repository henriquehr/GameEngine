#pragma once

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/glm.hpp"

#include <cassert>
#include <limits>

class Camera {
  public:
    void setPerspectiveProjection(float fovy, float aspect, float near, float far);

    void setView(glm::vec3 position, glm::vec3 front, glm::vec3 up);

    const glm::mat4 &getProjection() const {
        return projectionMatrix;
    }
    const glm::mat4 &getView() const {
        return viewMatrix;
    }
    const glm::mat4 &getInverseView() const {
        return inverseViewMatrix;
    }

  private:
    glm::mat4 projectionMatrix{1.0f};
    glm::mat4 viewMatrix{1.0f};
    glm::mat4 inverseViewMatrix{1.f};
};
