#pragma once

#include <string>
#include <map>
#include <vector>

#include "utils/glm_utils.h"
#include "utils/gl_utils.h"
#include "components/simple_scene.h"
#include "core/engine.h"
#include "core/gpu/mesh.h"
#include "../lab4/transform3D.h"

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
        virtual void Update(float angle, float scaleX, float scaleY);
        virtual bool ContainsPoint(glm::vec2 point);
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
        ~Complex();

        bool visible;
        glm::vec2 position;
        Rect box;
        float angle, scaleX, scaleY;

        virtual bool PointInBox(glm::vec2 point);
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

    class Score : public Complex {
    private:
        static constexpr float WIDTH = 136, HEIGHT = 20, MARGIN_RIGHT = 4, MARGIN_TOP = 145;
    public:
        static constexpr int MAX_SCORE = 50;
        int score, highScore, maxScore;

        Score(std::unordered_map<std::string, Mesh*>& worldMeshMap, int maxScore = MAX_SCORE);
        void Update(float deltaTime, int screenW, int screenH) override;
    };

    class Timer : public Complex {
    public:
        float cover;

        Timer(std::unordered_map<std::string, Mesh*>& worldMeshMap, int screenW);
        void Update(float deltaTime, int screenW, int screenH) override;
    };

    class Bullet : public Complex {
    private:
        static constexpr float SCALE_MIN = .1f;
        float timer;
    public:
        static constexpr float TIME_TO_HIT = .2;

        Bullet(std::unordered_map<std::string, Mesh*>& worldMeshMap);
        virtual void Shoot(glm::vec2 point);
        void Update(float deltaTime, int screenW, int screenH) override;
    };

    class Duck : public Complex {
    private:
        // Configurare rata
        static constexpr float baseSpeed = 150.0f, baseFlapSpeed = 7.0f, flapWideness = .25f;
        float flapAngle, animationTimer, startingHeight, randomTimer;
        // 0 - zboara, 1 - moare (RIP), 2 - a evadat, bravo!
        int status;

    public:
        // Util sa faca rata mai turbo cand se ajunge la un scor mare
        int score;

        Duck(std::unordered_map<std::string, Mesh*>& worldMeshMap);

        virtual bool HeadingRight();
        virtual void SetStatus(int newStatus, int screenW, int screenH);
        void Update(float deltaTime, int screenW, int screenH) override;
    };
}