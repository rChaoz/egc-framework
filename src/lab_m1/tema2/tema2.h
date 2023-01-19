#pragma once

#include <vector>

#include "components/simple_scene.h"
#include "objects.h"
#include "lab_m1/lab4/transform3D.h"

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;
     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderGround();
        void RenderTexturedMesh(Mesh* mesh, Texture2D* texture, const glm::mat4& modelMatrix = glm::mat4(1));
        void RenderMeshOwnTexture(Mesh* mesh, const glm::mat4& modelMatrix = glm::mat4(1));
        void RenderColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix = glm::mat4(1), const glm::vec3& color = glm::vec3(2, 2, 2));
        void RenderComplex(std::string name, float deltaTime, const glm::mat4 modelMatrix = glm::mat4(1));
        void RenderComplex(tema2::Complex* c, float deltaTime, const glm::mat4 modelMatrix = glm::mat4(1));
        void SendUniforms();

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        static constexpr float TRACK_WIDTH = 5.f;

        std::unordered_map<std::string, Texture2D*> mapTextures;
        std::unordered_map<std::string, tema2::Complex*> complexObjects;
    };
}   // namespace m1
