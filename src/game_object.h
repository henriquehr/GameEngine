#pragma once

#include "model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{};

    glm::mat4 modelMatrix();

    glm::mat3 normalMatrix();

    glm::mat4 calculateRotationMatrix(glm::mat4 &matrix);
};

struct PointLightComponent {
    float lightIntensity = 1.0f;
};

class GameObject {
  public:
    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, GameObject>;

    glm::vec3 color{};
    TransformComponent transform{};

    std::shared_ptr<Model> model{};
    std::unique_ptr<PointLightComponent> pointLight = nullptr;

    int textureIndex = 0;

    static GameObject createGameObject() {
        static id_t currentId = 0;
        return GameObject{currentId++};
    }

    static GameObject makePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

    id_t getId() {
        return id;
    }

  private:
    GameObject(id_t objId) : id(objId) {}

    id_t id;
};
