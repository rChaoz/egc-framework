#pragma once

#include <string>
#include <map>
#include <vector>

#include "utils/glm_utils.h"
#include "utils/gl_utils.h"
#include "components/simple_scene.h"
#include "core/engine.h"
#include "core/gpu/mesh.h"

namespace tema1 {
    Mesh* CreateRect(const std::string &name, glm::vec3 center, float width, float height, glm::vec3 color, bool fill = true, float zIndex = 0);
    Mesh* CreateCircle(const std::string &name, glm::vec3 center, float radius, glm::vec3 color, bool fill = true, float zIndex = 0);
    Mesh* CreateTriangle(const std::string& name, glm::vec3 center, glm::vec3 p1Delta, glm::vec3 p2Delta, glm::vec3 p3Delta, glm::vec3 color, bool fill = true, float zIndex = 0);

    class Rect {
    public:
        std::vector<glm::vec3> points;
        float top, right, bottom, left;

        Rect();
        Rect(float top, float right, float bottom, float left);
    };

    class Complex {
    public:
        std::vector<std::string> meshes;
        std::unordered_map<std::string, glm::mat3> meshMatrixes;

        Complex();

        glm::vec3 position;
        Rect box;
        float angle, scaleX, scaleY;

        virtual bool PointInBox(float x, float y);
        virtual void AddMesh(std::string mesh, glm::mat3 modelMatrix = glm::mat3(1));
        virtual void Update(float deltaTime, int screenW, int screenH);
    };

    class Duck : public Complex {
    private:
        // Configurare rata
        static constexpr float flapWideness = .25f;
        float flapAngle, animationTimer, startingHeight;
        // 0 - zboara, 1 - moare (RIP), 2 - a evadat, bravo!
        int status;

    public:
        static constexpr float baseSpeed = 150.0f;
        float speed;

        Duck();
        virtual bool HeadingRight();
        virtual void SetStatus(int newStatus, int screenW, int screenH);
        void Update(float deltaTime, int screenW, int screenH) override;
    };
}