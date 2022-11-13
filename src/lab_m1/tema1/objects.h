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

    struct ComplexMesh {
        glm::mat3 modelMatrix;
        bool visible;
        ComplexMesh(glm::mat3 modelMatrix = glm::mat3(1), bool visible = true);
    };

    class Complex {
    private:
        std::unordered_map<std::string, Mesh*>& const worldMeshMap;
    public:
        std::unordered_map<std::string, ComplexMesh> meshes;

        Complex(std::unordered_map<std::string, Mesh*>& worldMeshMap);

        bool visible;
        glm::vec3 position;
        Rect box;
        float angle, scaleX, scaleY;

        virtual bool PointInBox(float x, float y);
        virtual void AddMesh(std::string id, glm::mat3 modelMatrix = glm::mat3(1), bool visible = true);
        virtual void AddMesh(Mesh* mesh, glm::mat3 modelMatrix = glm::mat3(1), bool visible = true);
        virtual void Update(float deltaTime, int screenW, int screenH);
    };

    class StartCountdown : public Complex {
    private:
        float timerTotal, timer;

    public:
        StartCountdown(std::unordered_map<std::string, Mesh*>& worldMeshMap);

        virtual void Start(float timer);
        void Update(float deltaTime, int screenW, int screenH) override;
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

        Duck(std::unordered_map<std::string, Mesh*>& worldMeshMap);

        virtual bool HeadingRight();
        virtual void SetStatus(int newStatus, int screenW, int screenH);
        void Update(float deltaTime, int screenW, int screenH) override;
    };
}