#pragma once

#include "components/simple_scene.h"
#include <map>
#include "objects.h"

namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:
        Tema1();
        ~Tema1();

        void Init() override;

     private:
        std::unordered_map<std::string, tema1::Complex*> complexObjects;

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void ResetGame();
        glm::vec3 MouseToScreen(int mouseX, int mouseY);
        void RenderComplex(std::string name, float deltaTime, glm::vec3 deltaPosition = glm::vec3(0), bool customColor = false, glm::vec3 color = glm::vec3(0));
        void SetStatus(int status);
     protected:
         int SCREEN_W, SCREEN_H, status, score, hp, bullets;
         float shotTimer, spawnTimer, playerTimer, maxTimer;
         /* Statuses:
         * -2 - game over
         * -1 - game starting
         *  0 - game playing
         *  1 - duck dying
         *  2 - duck escaping
         */
    };
}   // namespace m1
