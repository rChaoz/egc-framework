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
        player->scale = glm::vec3(.05);
        player->position.y = 1.2f;

        Mesh* skis = new Mesh("skis");
        skis->LoadMesh(sourceModelsDir, "Ski.stl");
        player->AddMesh(skis, transform3D::Translate(-6.1f, -18, 11) * transform3D::RotateOY(-M_PI_2));

        Mesh* kratos = new Mesh("kratos");
        kratos->LoadMesh(sourceModelsDir, "kratos.stl");
        player->AddMesh(kratos, transform3D::Scale(.33f) * transform3D::RotateOX(M_PI_2));

        complexObjects["player"] = player;
    }
    // Obstacles
    {
        // Oildrum
        Obstacle* oildrum = new Obstacle(meshes, &speedV, true);
        oildrum->position.y = .25f;
        oildrum->ownSpeed.y = 5;

        Mesh* mesh = new Mesh("oildrum");
        mesh->LoadMesh(PATH_JOIN(RESOURCE_PATH::MODELS, "props"), "oildrum.obj");
        oildrum->AddMesh(mesh, transform3D::Translate(-.5f, 0, 0) * transform3D::RotateOX(-M_PI_2));

        complexObjects["oildrum"] = oildrum;
    }
    {
        // Tree
        Obstacle* tree = new Obstacle(meshes, &speedV);

        Mesh* mesh = new Mesh("tree");
        mesh->LoadMesh(sourceModelsDir, "RenderCrate-Dead_Tree_1.obj");
        tree->AddMesh(mesh, transform3D::Scale(.07f));

        complexObjects["tree"] = tree;
    }
    {
        // Lightpost
        Obstacle* lightpost = new Obstacle(meshes, &speedV, false, glm::vec3(.3f, .3f, .35f));

        Mesh* mesh = new Mesh("lightpost");
        mesh->LoadMesh(sourceModelsDir, "Lightpost.stl");
        lightpost->AddMesh(mesh, transform3D::Translate(-1.75f, 0, 0) * transform3D::RotateOZ(-M_PI_2) * transform3D::Scale(.04f));

        complexObjects["lightpost"] = lightpost;
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
            glm::vec3(0, 1, 1),
            glm::vec3(1, 0, 1),
            glm::vec3(1, 0, 0),
            glm::vec3(0, 1, 0)
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
    float angle = oldAngle + sign * ROTATION_SPEED * deltaTimeSeconds;
    if ((sign > 0 && angle > targetAngle) || (sign < 0 && angle < targetAngle)) angle = targetAngle;
    // Set player rotation & movement
    player->angle.y = angle;
    speedV.x = -sinf(angle) * speed;
    speedV.y = cosf(angle) * speed;
    // Obstacle spawning
    if (rand() % 1000 < SPAWN_CHANCE * speed) {
        Obstacle* obstacle = NULL;
        switch (rand() % 3) {
        case 0: // Oildrum
            obstacle = static_cast<Obstacle*>(complexObjects["oildrum"])->New(glm::vec2(rand() % 60 - 30, 25));
            break;
        case 1:
            obstacle = static_cast<Obstacle*>(complexObjects["tree"])->New(glm::vec2(rand() % 60 - 30, 25));
            break;
        case 2:
            obstacle = static_cast<Obstacle*>(complexObjects["lightpost"])->New(glm::vec2(rand() % 60 - 30, 25));
            break;
        }
        for (auto other : obstacles) {
            if (glm::length(other->position - obstacle->position) < 50 / speed) {
                delete obstacle;
                obstacle = NULL;
                break;
            }
        }
        if (obstacle != NULL) obstacles.push_back(obstacle);
    }
    // Remove obstacles gone too far
    auto it = obstacles.begin();
    while (it != obstacles.end()) {
        if ((*it)->position.z < -25) it = obstacles.erase(it);
        ++it;
    }

    // Update position * speed
    position += deltaTimeSeconds * speedV;
    speed += ACCELERATION;
    speedV *= (speed) / (speed - ACCELERATION);


    // Collision check

    // RENDERING
    
    //{
    //    glm::mat4 modelMatrix = glm::mat4(1);
    //    modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, -3));
    //    modelMatrix = glm::scale(modelMatrix, glm::vec3(2));
    //    RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, mapTextures["earth"], mapTextures["grass"]);
    //}
    // 

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
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

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
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

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
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

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

    // Call the mesh's render function
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));
    mesh->Render();
}

void Tema3::RenderColoredMesh(Mesh* mesh, const glm::mat4& modelMatrix, const glm::vec3& color) {
    const auto shader = shaders["default"];
    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

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
