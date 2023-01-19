#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;
using namespace tema2;


Tema2::Tema2() {
}


Tema2::~Tema2() {
}



void Tema2::Init() {
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "textures");
    const string sourceShaderDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders");

    // Create ground
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grass.jpg").c_str(), GL_REPEAT);
        mapTextures["ground"] = texture;

        vector<glm::vec3> vertices
        {
            glm::vec3(100,  0,  100),    // top right
            glm::vec3(100,  0, -100),    // bottom right
            glm::vec3(-100, 0, -100),    // bottom left
            glm::vec3(-100, 0,  100),    // top left
        };

        vector<glm::vec3> normals
        {
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, 1, 0),
        };

        vector<glm::vec2> textureCoords
        {
            glm::vec2(100, 100),
            glm::vec2(100, 0),
            glm::vec2(0, 0),
            glm::vec2(0, 100),
        };

        vector<unsigned int> indices =
        {
            0, 1, 3,
            1, 2, 3
        };

        Mesh* mesh = new Mesh("ground");
        mesh->InitFromData(vertices, normals, textureCoords, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("default");
        shader->AddShader(PATH_JOIN(sourceShaderDir, "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(sourceShaderDir, "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void Tema2::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds) {
    // RENDERING

    SendUniforms(shaders["default"]);
    RenderGround();
}


void Tema2::FrameEnd() {
}

void Tema2::RenderGround() {
    const auto shader = shaders["default"];
    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["ground"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));

    // Draw the object
    const auto* mesh = meshes["ground"];
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderTexturedMesh(Mesh* mesh, Texture2D* texture, const glm::mat4& modelMatrix) {
    const auto shader = shaders["default"];
    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderMeshOwnTexture(Mesh* mesh, const glm::mat4& modelMatrix) {
    const auto shader = shaders["default"];
    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Call the mesh's render function
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    mesh->Render();
}

void Tema2::RenderColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color) {
    const auto shader = shaders["default"];
    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 0);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(color));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema2::RenderComplex(std::string name, float deltaTime, const glm::mat4 modelMatrix) {
    RenderComplex(complexObjects[name], deltaTime, modelMatrix);
}

void Tema2::RenderComplex(Complex* c, float deltaTime, const glm::mat4 modelMatrix) {
    c->Update(deltaTime);
    if (!c->visible) return;

    auto parentMatrix = c->GetModelMatrix();
    for (auto& mesh : c->meshes) {
        if (!mesh.second.visible) continue;
        auto matrix = modelMatrix * parentMatrix * mesh.second.modelMatrix;
        if (mesh.second.texture != NULL) RenderTexturedMesh(meshes[mesh.first], mesh.second.texture, matrix);
        else if (c->overrideColor.x > 1 || c->overrideColor.y > 1 || c->overrideColor.z > 1) RenderMeshOwnTexture(meshes[mesh.first], matrix);
        else RenderColoredMesh(meshes[mesh.first], matrix, c->overrideColor);
    }
}

void Tema2::SendUniforms(Shader* shader) {
    // Apply shaders
    shader->Use();

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}


void Tema2::OnInputUpdate(float deltaTime, int mods) {
}


void Tema2::OnKeyPress(int key, int mods) {
}


void Tema2::OnKeyRelease(int key, int mods) {
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Tema2::OnWindowResize(int width, int height) {
}
