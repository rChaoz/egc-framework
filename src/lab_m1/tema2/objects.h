#pragma once

#include <string>
#include <map>
#include <vector>

#include "utils/glm_utils.h"
#include "utils/gl_utils.h"
#include "components/simple_scene.h"
#include "core/engine.h"
#include "core/gpu/mesh.h"

namespace tema2 {
    struct ComplexMesh {
        glm::mat4 modelMatrix;
        bool visible;
        Texture2D* texture;
        ComplexMesh(glm::mat4 modelMatrix = glm::mat4(1), Texture2D *texture = NULL, bool visible = true);
    };

    class Complex {
    protected:
        std::unordered_map<std::string, Mesh*>& const worldMeshMap;
        bool deleteMeshes;

    public:
        std::unordered_map<std::string, ComplexMesh> meshes;

        Complex(std::unordered_map<std::string, Mesh*>& worldMeshMap, glm::vec3 &overrideColor = glm::vec3(2, 2, 2));
        ~Complex();

        bool visible;
        float radius;
        glm::vec3 position, angle, scale, overrideColor;

        virtual glm::mat4 GetModelMatrix();
        virtual glm::vec3 Forward();
        virtual bool Touches(const Complex* another);
        virtual void AddMesh(std::string id, glm::mat4 modelMatrix = glm::mat4(1), Texture2D* texture = NULL, bool visible = true);
        virtual void AddMesh(Mesh* mesh, glm::mat4 modelMatrix = glm::mat4(1), Texture2D* texture = NULL, bool visible = true);
        virtual void Update(float deltaTime);
    };
    
}