
#include "game_object.h"

glm::mat4 TransformComponent::calculateRotationMatrix(glm::mat4 &matrix) {
    matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    return matrix;
}

glm::mat4 TransformComponent::modelMatrix() {
    glm::mat4 matrix{1.0f};
    matrix = glm::translate(matrix, translation);
    matrix = calculateRotationMatrix(matrix);
    matrix = glm::scale(matrix, scale);
    return matrix;
}

glm::mat3 TransformComponent::normalMatrix() {
    glm::mat4 matrix{1.0f};
    matrix = calculateRotationMatrix(matrix);
    matrix = glm::scale(matrix, 1.0f / scale);
    return matrix;
}

GameObject GameObject::makePointLight(float intensity, float radius, glm::vec3 color) {
    GameObject gameObj = GameObject::createGameObject();
    gameObj.color = color;
    gameObj.transform.scale.x = radius;
    gameObj.pointLight = std::make_unique<PointLightComponent>();
    gameObj.pointLight->lightIntensity = intensity;
    return gameObj;
}
