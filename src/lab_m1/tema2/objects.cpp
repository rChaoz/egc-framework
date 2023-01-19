#include "objects.h"
#include "lab_m1/lab4/transform3D.h"

#include <iostream>

using namespace tema2;

// COMPLEX MESH
ComplexMesh::ComplexMesh(glm::mat4 modelMatrix, Texture2D *texture, bool visible) : modelMatrix(modelMatrix), visible(visible), texture(texture) {}

// COMPLEX
Complex::Complex(std::unordered_map<std::string, Mesh*>& worldMeshMap, glm::vec3& overrideColor)
    : worldMeshMap(worldMeshMap), overrideColor(overrideColor), radius(0), deleteMeshes(true), visible(true) {
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
