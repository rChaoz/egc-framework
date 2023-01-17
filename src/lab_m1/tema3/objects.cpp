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
Obstacle::Obstacle(std::unordered_map<std::string, Mesh*>& worldMeshMap, glm::vec2* speed, const int type)
    : Complex(worldMeshMap), speed(speed), falling(false), type(type), timer(0) {
    deleteMeshes = false;
    switch (type) {
    case BARREL:
        radius = .5f;
        position.y = .25f;
        AddMesh(worldMeshMap["oildrum"], transform3D::Translate(-.5f, 0, 0) * transform3D::RotateOX(-M_PI_2));
        break;
    case TREE:
        radius = .4f;
        AddMesh(worldMeshMap["tree"], transform3D::Scale(.07f));
        break;
    case LIGHTPOST:
        overrideColor = glm::vec3(.3f, .3f, .35f);
        radius = .3f;
        AddMesh(worldMeshMap["lightpost"], transform3D::Translate(-1.6f, 0, 0) * transform3D::RotateOZ(-M_PI_2) * transform3D::Scale(.04f));
        break;
    case COIN:
        overrideColor = glm::vec3(1, .85, .21);
        radius = .6f;
        AddMesh(worldMeshMap["coin"], transform3D::Translate(0, 1, 0) * transform3D::Scale(.8f) * transform3D::RotateOZ(M_PI_2));
        break;
    }
}

void Obstacle::Update(float deltaTime) {
    timer += deltaTime;
    glm::vec2 delta = deltaTime * *speed;
    if (type == BARREL) {
        delta.y += deltaTime * BARREL_SPEED;
        angle.z -= delta.y;
    }
    else if (type == COIN) {
        angle.y += deltaTime * 4;
        position.y = sinf(timer * 6) * .3f;
    }
    position.x -= delta.x;
    position.z -= delta.y;
    if (falling) {
        position.y -= deltaTime * 5;
        angle.x += deltaTime * 2;
    }
}

bool Obstacle::Touches(const Complex* other) {
    if (falling) return false;
    else return Complex::Touches(other);
}