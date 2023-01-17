#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"
#include "objects.h"
#include "lab_m1/lab4/transform3D.h"


namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
     public:
        Tema3();
        ~Tema3();

        void Init() override;

     private:

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderGround();
        void RenderTexturedMesh(Mesh *mesh, Texture2D *texture, const glm::mat4 &modelMatrix = glm::mat4(1));
        void RenderMeshOwnTexture(Mesh* mesh, const glm::mat4& modelMatrix = glm::mat4(1));
        void RenderColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix = glm::mat4(1), const glm::vec3& color = glm::vec3(2, 2, 2));
        void RenderComplex(std::string name, float deltaTime, const glm::mat4 finalTransform = glm::mat4(1));
        void RenderComplex(tema3::Complex *c, float deltaTime, const glm::mat4 finalTransform = glm::mat4(1));
        void SendUniforms(Shader* shader, const glm::mat4& modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        static constexpr float ROTATION_SPEED = 1.f;
        static constexpr float ACCELERATION = .003f;
        static constexpr int SPAWN_CHANCE = 5;

        std::unordered_map<std::string, Texture2D *> mapTextures;
        std::unordered_map<std::string, tema3::Complex*> complexObjects;
        std::list<tema3::Obstacle*> obstacles;

        float cameraShake;

        glm::vec2 position;
        glm::vec2 speedV;
        float speed, mouseX;
    };
}   // namespace m1
