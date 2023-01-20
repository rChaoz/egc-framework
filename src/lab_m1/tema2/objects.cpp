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

glm::vec3 Complex::Forward() {
    return glm::vec3(-sinf(angle.y), 0, cosf(angle.y));
}

void Complex::SetForward(glm::vec3 forward) {
    forward = glm::normalize(forward);
    angle.y = acosf(angle.z);
    if (forward.x > 0) angle.y = 2 * M_PI - angle.y;
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

Obstacle::Obstacle(std::vector<glm::vec2>& track, float trackWidth) : color(rand() % 100 / 100.f, rand() % 100 / 100.f, rand() % 100 / 100.f),
    track(track), speed(rand() % 100 / 10.f + 5), trackWidth(trackWidth * .4f), sidewaysPos(rand() % 100 / 50.f - 1) {
    point = rand() % track.size();
    delta = length = 0;
    Update(.1f);
}

void Obstacle::Update(float deltaTime) {
    delta += speed * deltaTime;
    if (delta > length) {
        delta -= length;
        if (--point < 0) point = track.size() - 1;

        glm::vec2 from = track[point], to = track[point == 0 ? track.size() - 1 : point - 1];
        dir = glm::normalize(to - from);
        length = glm::length(to - from);

        angle = acosf(dir.y);
        if (dir.x > 0) angle = 2 * M_PI - angle;
    }
}

glm::vec3 Obstacle::GetPosition() {
    const auto pos2 = track[point] + delta * dir + glm::normalize(glm::vec2(-dir.y, dir.x)) * sidewaysPos * trackWidth;
    return glm::vec3(pos2.x, .65f, pos2.y);
}

glm::mat4 Obstacle::GetModelMatrix() {
    return transform3D::Translate(GetPosition()) * transform3D::RotateOY(angle);
}