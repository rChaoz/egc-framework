#include "lab_m1/Tema2/Tema2.h"
#include "components/transform.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;
using namespace m1;
using namespace tema2;


Tema2::Tema2() {
    speed = 0;
}


Tema2::~Tema2() {
}



void Tema2::Init() {
    // Init camera
    camera = new implemented::Camera();

    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "textures");
    const string sourceModelsDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "models");
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
        AddMeshToList(mesh);
    }

    // Create track
    {
        // Load texture
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "track.jpg").c_str(), GL_REPEAT);
        mapTextures["track"] = texture;

        // Variables
        vector<glm::vec2> vertices;
        vector<unsigned int> indices;

        // Read & process points file
        ifstream points(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "track.txt").c_str());
        string p1, p2;
        // Read first point
        points >> p1 >> p2;
        glm::vec2 from(stof(p1), stof(p2)), to, pFrom, pTo;
        glm::vec2 start = from, pStart, first;
        // 3 - FIRST
        // 1 - LAST (connect last point to first)
        // 0 - MISSING (connect missing edge)
        int status = 3;

        while (points >> p1 >> p2 || (status--)) {
            to.x = stof(p1), to.y = stof(p2);
            if (status == 3) {
                --status;
                // Just find perpendicular
                pFrom = to - from;
                pStart = pFrom = glm::normalize(glm::vec2(-pFrom.y, pFrom.x)) * (TRACK_WIDTH / 2);
                first = from = to;
                // Set starting point
                startingPosition = glm::vec3(start.x, 0, start.y);
                glm::vec2 forward = first - start;
                startingForward = glm::vec3(forward.x, 0, forward.y);
                continue; // don't draw any lines
            }
            else if (status == 1) {
                // Connect last point to the first
                to = start;
            }
            else if (status == 0) {
                // Connect missing line (not drawn for first point)
                to = first;
            }
            // Find perpendicular
            pTo = to - from;
            pTo = glm::normalize(glm::vec2(-pTo.y, pTo.x)) * (TRACK_WIDTH / 2);

            // Find point farthest from 'to' between ending points of last segment
            glm::vec2 from1 = from + pFrom, from2 = from - pFrom;
            if (glm::length(to - from1) > glm::length(to - from2)) {
                // Closer to from1
                from2 = from1 - pTo * 2.f;
            }
            else {
                // Closer to from2
                from1 = from2 + pTo * 2.f;
            }
            vertices.push_back(from1);
            vertices.push_back(from2);
            // Create line (rectangle) starting at 'from' ending at 'to'
            int splits = glm::length(to - from) / .1f;
            glm::vec2 to1 = to + pTo, to2 = to - pTo;
            glm::vec2 dir1 = (to1 - from1) / static_cast<float>(splits);
            glm::vec2 dir2 = (to2 - from2) / static_cast<float>(splits);

            glm::vec2 p1 = from1, p2 = from2; // points 1 & 2
            for (int i = 0; i < splits; ++i) {
                glm::vec2 n1 = p1 + dir1, n2 = p2 + dir2; // next points 1 & 2
                int offset = vertices.size();
                vertices.push_back(n1); // offset     (offset - 2 for next point)
                vertices.push_back(n2); // offset + 1 (offset - 1 for next point)

                indices.push_back(offset - 2), indices.push_back(offset), indices.push_back(offset + 1);
                indices.push_back(offset - 2), indices.push_back(offset + 1), indices.push_back(offset - 1);

                p1 = n1, p2 = n2;
            }

            // Update vars that hold previous-point values
            from = to;
            pFrom = pTo;
        }

        // Create mesh
        Mesh* mesh = new Mesh("track");
        vector<glm::vec3> verts, normals;
        vector<glm::vec2> textureCoords;
        vector<VertexFormat> vf;
        bool change = false;
        for (auto v : vertices) {
            verts.push_back(glm::vec3(v.x, .1f, v.y));
            normals.push_back(glm::vec3(0, 1, 0));
            textureCoords.push_back(v / 5.f);
        }
        //mesh->InitFromData(v, indices);
        mesh->InitFromData(verts, normals, textureCoords, indices);
        AddMeshToList(mesh);
    }

    // Create car
    {
        Complex* car = new Complex(meshes, glm::vec3(.6f, .26f, .08f));
        Mesh* mesh = new Mesh("car");
        mesh->LoadMesh(sourceModelsDir, "car.stl");
        car->AddMesh(mesh, transform3D::Translate(.68f, 4.1f, -5.5f) * transform3D::Scale(.05f) * transform3D::Rotate(0, M_PI_2, M_PI));
        complexObjects["car"] = car;

        car->position = startingPosition;
        car->SetForward(startingForward);
    }

    // Load shaders
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
    glClearColor(0, 0.03f, 0.15f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds) {
    // GAME LOGIC
    const auto car = complexObjects["car"];
    car->position += car->Forward() * speed * deltaTimeSeconds;
    
    // Camera follows car
    camera->Set(car->position + glm::vec3(0, 5, 0) - car->Forward() * 6.f, car->position, glm::vec3(0, 1, 0));

    // RENDERING
    SendUniforms();
    RenderGround();
    glUniform1i(glGetUniformLocation(shaders["default"]->program, "shinyness"), 2);
    RenderComplex("car", deltaTimeSeconds);
}


void Tema2::FrameEnd() {
}

void Tema2::RenderGround() {
    const auto shader = shaders["default"];
    shader->Use();
    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniform1i(glGetUniformLocation(shader->program, "useTexture"), 1);
    glUniform3fv(glGetUniformLocation(shader->program, "overrideColor"), 1, glm::value_ptr(glm::vec3(2, 2, 2)));

    // Render ground
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mapTextures["ground"]->GetTextureID());
    glUniform1i(glGetUniformLocation(shader->program, "u_texture_0"), 0);
    auto mesh = meshes["ground"];
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);

    // And track
    glBindTexture(GL_TEXTURE_2D, mapTextures["track"]->GetTextureID());
    mesh = meshes["track"];
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

void Tema2::SendUniforms(bool freeCam) {
    // Apply shaders
    const auto shader = shaders["default"];
    shader->Use();

    // Bind view matrix
    glm::mat4 viewMatrix = freeCam ? GetSceneCamera()->GetViewMatrix() : camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = freeCam ? GetSceneCamera()->GetProjectionMatrix() : glm::perspective(RADIANS(90), window->props.aspectRatio, 0.01f, 300.0f);
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Bind camera position & shinyness
    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(GetSceneCamera()->m_transform->GetWorldPosition()));
    glUniform1i(glGetUniformLocation(shader->program, "shinyness"), 1);
}


void Tema2::OnInputUpdate(float deltaTime, int mods) {
    if (window->KeyHold(GLFW_KEY_W)) speed += ACCELERATION * deltaTime;
    else if (window->KeyHold(GLFW_KEY_S)) {
        if (speed > 0) speed = max(speed - BREAK * deltaTime, 0.f);
        else speed -= ACCELERATION * deltaTime;
    }
    else {
        if (speed > 0) speed = max(speed - SLOW * deltaTime, 0.f);
        else if (speed < 0) speed = min(speed + SLOW * deltaTime, 0.f);
    }

    if (speed > TOP_SPEED) speed = TOP_SPEED;
    else if (speed < -TOP_REVERSE_SPEED) speed = -TOP_REVERSE_SPEED;

    const auto car = complexObjects["car"];
    const float turn = min(deltaTime * speed * TURN_RATIO, MAX_TURN);
    if (window->KeyHold(GLFW_KEY_A)) car->angle.y -= turn;
    if (window->KeyHold(GLFW_KEY_D)) car->angle.y += turn;
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
