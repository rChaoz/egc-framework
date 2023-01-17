#include "lab_m1/Tema3/Tema3.h"
#include "lab_m1/lab5/lab_camera.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;
using namespace tema3;

Tema3::Tema3() {
    mouseX = 0;
    position.x = position.y = speedV.x = 0;
    speedV.y = speed = 6;
    cameraShake = 1;
    score = coins = 0;
}


Tema3::~Tema3() {
}

void Tema3::Init()
{
    // Setup camera
    //GetCameraInput()->SetActive(false);
    GetSceneCamera()->SetPosition(glm::vec3(0, 12, 22));
    GetSceneCamera()->RotateOX(-300);
    GetSceneCamera()->Update();
    
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "textures");
    const string sourceModelsDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "models");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "snow.jpg").c_str(), GL_REPEAT);
        mapTextures["ground"] = texture;
    }

    // Load meshes
    {
        // Player
        Complex* player = new Complex(meshes, glm::vec3(.7, .36, .1)); // glm::vec3(1, .56, .1)
        player->radius = .7f;
        player->scale = glm::vec3(.05);
        player->position.y = .2f;

        Mesh* skis = new Mesh("skis");
        skis->LoadMesh(sourceModelsDir, "Ski.stl");
        player->AddMesh(skis, transform3D::Translate(-6.1f, 2, 11) * transform3D::RotateOY(-M_PI_2));

        Mesh* kratos = new Mesh("kratos");
        kratos->LoadMesh(sourceModelsDir, "kratos.stl");
        player->AddMesh(kratos, transform3D::Translate(0, 20, 0) * transform3D::Scale(.33f) * transform3D::RotateOX(M_PI_2));

        complexObjects["player"] = player;
    }
    // Load obstacle meshes
    {
        Mesh* oildrum = new Mesh("oildrum");
        oildrum->LoadMesh(PATH_JOIN(RESOURCE_PATH::MODELS, "props"), "oildrum.obj");
        meshes[oildrum->GetMeshID()] = oildrum;

        Mesh* tree = new Mesh("tree");
        tree->LoadMesh(sourceModelsDir, "RenderCrate-Dead_Tree_1.obj");
        meshes[tree->GetMeshID()] = tree;

        Mesh* lightpost = new Mesh("lightpost");
        lightpost->LoadMesh(sourceModelsDir, "Lightpost.stl");
        meshes[lightpost->GetMeshID()] = lightpost;

        Mesh* coin = new Mesh("coin");
        coin->LoadMesh(sourceModelsDir, "coin.obj");
        meshes[coin->GetMeshID()] = coin;
    }

    // Create  ground
    {
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

    // Shaders
    {
        Shader *shader = new Shader("default");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
    {
        Shader* shader = new Shader("ground");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "GroundVertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds) {
    // GAME LOGIC
    
    // Update based on mouse position
    const auto player = complexObjects["player"];
    const float w = window->GetResolution().x;
    float targetAngle = M_PI_4 - M_PI_2 * min(w, max(0.f, mouseX * 1.2f - w / 10)) / w;
    float oldAngle = player->angle.y, deltaAngle = targetAngle - oldAngle;
    // Calculate new angle
    float sign = deltaAngle < 0 ? -1 : deltaAngle > 0 ? 1 : 0;
    float angle = oldAngle + sign * max(1.f + speed / 20.f, 1.5f) * deltaTimeSeconds;
    if ((sign > 0 && angle > targetAngle) || (sign < 0 && angle < targetAngle)) angle = targetAngle;
    // Set player rotation & movement
    player->angle.y = angle;
    speedV.x = -sinf(angle) * speed;
    speedV.y = cosf(angle) * speed;
    // Obstacle spawning
    if (rand() % 1000 < SPAWN_CHANCE * speed) {
        Obstacle* obstacle = NULL;
        glm::vec3 spawnPos(rand() % 60 - 30, 0, 25);
        for (auto other : obstacles) {
            if (glm::length(other->position - spawnPos) < max(50 / speed, 4.f)) {
                spawnPos.y = -1;
                break;
            }
        }
        if (spawnPos.y == 0) {
            Obstacle* obstacle = obstacle = new Obstacle(meshes, &speedV, rand() % 4);
            obstacle->position += spawnPos; // dont use =, keep y value
            obstacles.push_back(obstacle);
        }
    }
    // Remove obstacles gone too far
    auto it = obstacles.begin();
    while (it != obstacles.end()) {
        if ((*it)->position.z < -25) it = obstacles.erase(it);
        else ++it;
    }

    // Update position * speed
    position += deltaTimeSeconds * speedV;
    speed += ACCELERATION;
    speedV *= (speed) / (speed - ACCELERATION);

    // Collision check
    for (auto obstacle : obstacles) if (obstacle->Touches(player)) {
        if (obstacle->type == Obstacle::COIN) {
            obstacle->position.z = -50; // will be removedd automatically
            ++coins;
            continue;
        }
        obstacle->falling = true;
        const float newSpeed = max(3.f, speed / 2);
        speedV *= newSpeed / speed;
        speed = newSpeed;
        if (cameraShake > .5f) cameraShake = 0;
    }

    // Camera shake
    if (cameraShake < .5f) {
        cameraShake += deltaTimeSeconds;
        if (cameraShake >= .5f) GetSceneCamera()->SetPosition(glm::vec3(0, 12, 22));
        else GetSceneCamera()->SetPosition(glm::vec3(sinf(cameraShake * 50) / 6, 12, 22));
        GetSceneCamera()->Update();
    }

    // RENDERING

    // Render player & ground
    RenderGround();
    RenderComplex("player", deltaTimeSeconds);
    // Render obstacles
    for (auto obstacle : obstacles) RenderComplex(obstacle, deltaTimeSeconds);
}


void Tema3::FrameEnd() {
}


void Tema3::RenderGround() {
    const auto shader = shaders["ground"];
    SendUniforms(shader, glm::mat4(1));

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["ground"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    glUniform2fv(glGetUniformLocation(shader->program, "textureCoordinatesDelta"), 1, glm::value_ptr(position / 2.f));

    // Draw the object
    const auto *mesh = meshes["ground"];
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderTexturedMesh(Mesh *mesh, Texture2D* texture, const glm::mat4& modelMatrix) {
    const auto shader = shaders["default"];
    SendUniforms(shader, modelMatrix);

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

void Tema3::RenderMeshOwnTexture(Mesh *mesh, const glm::mat4& modelMatrix) {
    const auto shader = shaders["default"];
    SendUniforms(shader, modelMatrix);

    // Call the mesh's render function
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    mesh->Render();
}

void Tema3::RenderColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color) {
    const auto shader = shaders["default"];
    SendUniforms(shader, modelMatrix);

    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 0);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(color));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void Tema3::RenderComplex(std::string name, float deltaTime, const glm::mat4 finalTransform) {
    RenderComplex(complexObjects[name], deltaTime, finalTransform);
}

void Tema3::RenderComplex(Complex* c, float deltaTime, const glm::mat4 finalTransform) {
    c->Update(deltaTime);
    if (!c->visible) return;

    auto parentMatrix = c->GetModelMatrix();
    for (auto& mesh : c->meshes) {
        if (!mesh.second.visible) continue;
        auto matrix = finalTransform * parentMatrix * mesh.second.modelMatrix;
        if (mesh.second.texture != NULL) RenderTexturedMesh(meshes[mesh.first], mesh.second.texture, matrix);
        else if (c->overrideColor.x > 1 || c->overrideColor.y > 1 || c->overrideColor.z > 1) RenderMeshOwnTexture(meshes[mesh.first], matrix);
        else RenderColoredMesh(meshes[mesh.first], matrix, c->overrideColor);
    }
}

void Tema3::SendUniforms(Shader* shader, const glm::mat4& modelMatrix) {
    // Apply shaders
    shader->Use();

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Lightposts
    glm::vec3 spotlights[800];
    int spotlightCount = 0;
    for (auto o : obstacles) if (o->type == Obstacle::LIGHTPOST && !o->falling) {
        const auto p = o->position;
        spotlights[spotlightCount] = glm::vec3(p.x, p.y + 4, p.z + 1.5f);
        ++spotlightCount;
    }
    glUniform1i(glGetUniformLocation(shader->program, "spotlightCount"), spotlightCount);
    glUniform3fv(glGetUniformLocation(shader->program, "spotlights"), spotlightCount, glm::value_ptr(spotlights[0]));

    // Player lantern & camera
    const float angle = complexObjects["player"]->angle.y;
    glUniform3fv(glGetUniformLocation(shader->program, "playerSpotlightPos"), 1, glm::value_ptr(glm::vec3(0, 2, .5f)));
    glUniform3fv(glGetUniformLocation(shader->program, "playerSpotlightDir"), 1, glm::value_ptr(glm::vec3(-2 * sinf(angle), -.5f, 2 * cosf(angle))));
    glUniform3fv(glGetUniformLocation(shader->program, "eyePosition"), 1, glm::value_ptr(glm::vec3(0, 12, 22)));
}

void Tema3::OnInputUpdate(float deltaTime, int mods) {
}


void Tema3::OnKeyPress(int key, int mods) {
}


void Tema3::OnKeyRelease(int key, int mods) {
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    this->mouseX = mouseX;
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Tema3::OnWindowResize(int width, int height) {
}
