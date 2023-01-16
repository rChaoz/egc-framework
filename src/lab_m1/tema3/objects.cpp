#include "objects.h"
#include "lab_m1/lab4/transform3D.h"

#include <iostream>

using namespace tema3;

// COMPLEX MESH
ComplexMesh::ComplexMesh(glm::mat4 modelMatrix, Texture2D *texture, bool visible) : modelMatrix(modelMatrix), visible(visible), texture(texture) {}

// COMPLEX
Complex::Complex(std::unordered_map<std::string, Mesh*>& worldMeshMap, glm::vec3& overrideColor)
    : worldMeshMap(worldMeshMap), overrideColor(overrideColor), radius(0), deleteMeshes(true) {
    visible = true;
    position = glm::vec3(0, 0, 0);
    angle = glm::vec3(0, 0, 0);
    scale = glm::vec3(1, 1, 1);
}

Complex::~Complex() {
    if (!deleteMeshes) return;
    for (auto mesh : meshes) {
        auto m = worldMeshMap[mesh.first];
        if (m) {
            delete m;
            worldMeshMap.erase(mesh.first);
        }
    }
}

glm::mat4 Complex::GetModelMatrix() {
    return transform3D::Translate(position.x, position.y, position.z)
        * transform3D::Rotate(angle.x, angle.y, angle.z)
        * transform3D::Scale(scale.x, scale.y, scale.z);
}

bool Complex::Touches(const Complex* another) {
    const float distance = glm::length(position - another->position);
    return distance < radius + another->radius;
}

void Complex::AddMesh(Mesh* mesh, glm::mat4 modelMatrix, Texture2D* texture, bool visible) {
    auto id = mesh->GetMeshID();
    worldMeshMap[id] = mesh;
    AddMesh(id, modelMatrix, texture);
}

void Complex::AddMesh(std::string id, glm::mat4 modelMatrix, Texture2D* texture, bool visible) {
    meshes[id] = ComplexMesh(modelMatrix, texture, visible);
}

void Complex::Update(float deltaTime) {}

// OBSTACLE
Obstacle::Obstacle(std::unordered_map<std::string, Mesh*>& worldMeshMap, glm::vec2* speed, bool rotate, glm::vec3& overrideColor)
    : Complex(worldMeshMap, overrideColor), speed(speed), rotate(rotate) {
    ownSpeed = glm::vec2(0);
}

void Obstacle::Update(float deltaTime) {
    glm::vec2 delta = deltaTime * (ownSpeed + *speed);
    position.x -= delta.x;
    position.z -= delta.y;
    if (rotate) angle.z -= deltaTime * (ownSpeed.y + speed->y);
}

Obstacle* Obstacle::New(glm::vec2& initialPosition) {
    auto *o = new Obstacle(worldMeshMap, speed, rotate, overrideColor);
    o->deleteMeshes = false;
    o->meshes = meshes;
    o->ownSpeed = ownSpeed;
    o->position = glm::vec3(initialPosition.x, position.y, initialPosition.y);
    o->angle = angle;
    o->scale = scale;
    return o;
}
