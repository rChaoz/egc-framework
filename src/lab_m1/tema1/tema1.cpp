#include "lab_m1/tema1/tema1.h"
#include "lab_m1/lab3/transform2D.h"

#include <vector>
#include <iostream>


using namespace std;
using namespace m1;

Tema1::Tema1() {
    SCREEN_W = 1000;
    SCREEN_H = 515;
}


Tema1::~Tema1() {
}


void Tema1::Init() {
    window->HidePointer();
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // Initialize ground & moon
    {
        // Ground
        glm::vec3 color(0.15f, 0.53f, 0.03f);
        AddMeshToList(tema1::CreateRect("ground", glm::vec3(SCREEN_W / 2, -550, 0), SCREEN_W, 1100, color, true, 10));

        // Grass
        std::vector<VertexFormat> vertices;
        std::vector<unsigned int> indices;

        int grassSpace = 11, grassBlades = SCREEN_W / grassSpace;
        for (int i = 0; i <= grassBlades; i++) {
            vertices.push_back(VertexFormat(glm::vec3(4 + rand() % 3 + i * grassSpace, 20 + rand() % 15, 10), color));
            vertices.push_back(VertexFormat(glm::vec3(i * grassSpace, 0, 10), color));
            vertices.push_back(VertexFormat(glm::vec3(10 + i * grassSpace, 0, 10), color));
            indices.push_back(i * 3);
            indices.push_back(i * 3 + 1);
            indices.push_back(i * 3 + 2);
        }

        Mesh* grass = new Mesh("grass");
        grass->InitFromData(vertices, indices);
        AddMeshToList(grass);

        // Moon
        auto* moon = new tema1::Complex(meshes);
        moon->AddMesh(tema1::CreateCircle("moon", glm::vec3(0), 50, glm::vec3(0.78f, 0.75f, 0.67f), true, -1.0f));
        moon->AddMesh(tema1::CreateCircle("moon_crater1", glm::vec3(18, 15, 0), 12, glm::vec3(0.58f, 0.56f, 0.5f), true, -.8f));
        moon->AddMesh(tema1::CreateCircle("moon_crater2", glm::vec3(-25, -19, 0), 4, glm::vec3(0.58f, 0.56f, 0.5f), true, -.8f));
        complexObjects["moon"] = moon;
    }
    // Cursor
    {
        auto *cursor = new tema1::Complex(meshes);
        glm::vec3 color(1, 0.28f, 0.28f);
        cursor->AddMesh(tema1::CreateRect("cursor_1", glm::vec3(0), 20, 3, color, true, 15.5f), transform2D::Rotate(M_PI_4));
        cursor->AddMesh(tema1::CreateRect("cursor_1_shadow", glm::vec3(0), 20, 3, glm::vec3(0), true, 15.0f), transform2D::Translate(2, -2) * transform2D::Rotate(M_PI_4));
        cursor->AddMesh(tema1::CreateRect("cursor_2", glm::vec3(0), 20, 3, color, true, 15.5f), transform2D::Rotate(-M_PI_4));
        cursor->AddMesh(tema1::CreateRect("cursor_2_shadow", glm::vec3(0), 20, 3, glm::vec3(0), true, 15.0f), transform2D::Translate(2, -2) * transform2D::Rotate(-M_PI_4));
        complexObjects["cursor"] = cursor;
    }
    // Bullet & heart objects
    {
        // Bullet
        auto* bullet = new tema1::Complex(meshes);
        bullet->AddMesh(tema1::CreateCircle("bullet_top", glm::vec3(0, 30, 0), 8, glm::vec3(0.53f, 0.3f, 0.1f), true, 5.0f));
        bullet->AddMesh(tema1::CreateRect("bullet_bot", glm::vec3(0, 15, 0), 16, 30, glm::vec3(0.82f, 0.53f, 0.19f), true, 5.5f));
        complexObjects["bullet"] = bullet;

        // Heart
        glm::vec3 color(0.89f, 0.17f, 0.18f);
        std::vector<VertexFormat> vertices = { VertexFormat(glm::vec3(0), color) };
        std::vector<unsigned int> indices = { 0 };

        // Center at 0,0
        int vertexNum = 0;
        // Bottom part
        for (int i = -20; i <= 20; i += 2) {
            float x = i;
            float y = (acosf(1 - abs(x / 10)) - M_PI) * 9;
            if (i == 0) y += 3;
            vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
            indices.push_back(++vertexNum);
        }
        // Right-top part
        for (int i = 0; i <= 10; i++) {
            float x = cosf(M_PI * i / 10) * 10 + 10, y = sinf(M_PI * i / 10) * 10;
            vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
            indices.push_back(++vertexNum);
        }
        // Left-top part
        for (int i = 0; i <= 10; i++) {
            float x = -cosf(M_PI * i / 10) * 10 - 10, y = sinf(M_PI * i / 10) * 10;
            vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
            indices.push_back(++vertexNum);
        }

        Mesh *heart = new Mesh("heart");
        heart->SetDrawMode(GL_TRIANGLE_FAN);
        heart->InitFromData(vertices, indices);
        AddMeshToList(heart);
    }
    // Player timer & score
    {
        glm::vec3 red(1, .1f, .1f), green(.1f, 1, .1f);
        std::vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(SCREEN_W / 3 + 1, -5, 10.2f), red),
            VertexFormat(glm::vec3(SCREEN_W * 2 / 3, -5, 10.2f), green),
            VertexFormat(glm::vec3(SCREEN_W * 2 / 3, 5, 10.2f), green),
            VertexFormat(glm::vec3(SCREEN_W / 3 + 1, 5, 10.2f), red)
        };
        std::vector<unsigned int> indices { 0, 1, 2, 2, 3, 0 };

        Mesh* timer = new Mesh("timer");
        timer->InitFromData(vertices, indices);
        AddMeshToList(timer);
        AddMeshToList(tema1::CreateRect("timer_cover", glm::vec3(-SCREEN_W / 6, 0, 0), SCREEN_W / 3, 10, glm::vec3(.1f), true, 10.5f));
        AddMeshToList(tema1::CreateRect("timer_outline", glm::vec3(SCREEN_W / 2, 0, 0), SCREEN_W / 3 + 4, 14, glm::vec3(.1f), true, 10.0f));
        // TODO score
    }
    // Game Starting / Over UI
    {
        glm::vec3 xColor(0.6f, 0.09f, 0.09f);
        tema1::Complex* gameOver = new tema1::Complex(meshes);
        gameOver->AddMesh(tema1::CreateRect("gameOver_1", glm::vec3(0), 250, 20, xColor, true, 20), transform2D::Rotate(AI_DEG_TO_RAD(45)));
        gameOver->AddMesh(tema1::CreateRect("gameOver_2", glm::vec3(0), 250, 20, xColor, true, 20), transform2D::Rotate(AI_DEG_TO_RAD(-45)));
        complexObjects["gameOver"] = gameOver;

        complexObjects["startCountdown"] = new tema1::StartCountdown(meshes);
    }
    // Create duck
    complexObjects["duck"] = new tema1::Duck(meshes);

    ResetGame();
}


void Tema1::FrameStart() {
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0.03f, 0.15f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
    SCREEN_H = SCREEN_W * resolution.y / resolution.x;
    GetSceneCamera()->SetOrthographic(0, SCREEN_W, 0, SCREEN_H, 0.01f, 400);
}


void Tema1::Update(float deltaTimeSeconds) {
    // Game logic
    shotTimer += deltaTimeSeconds;
    spawnTimer -= deltaTimeSeconds;
    const bool redDuck = shotTimer < 1;

    if (status != 0 && spawnTimer < 0) {
        if (hp > 0) SetStatus(0);
        else SetStatus(-2);
    }
    if (status == 0) {
        playerTimer -= deltaTimeSeconds;
        if (playerTimer < 0) SetStatus(2);
    }
    
    // Rendering

    {
        // Grass occupies 1/4 of the screen
        float grassHeight = SCREEN_H / 4;
        glm::mat3 modelMatrix = transform2D::Translate(0, grassHeight) * transform2D::Scale(1, grassHeight / 1000);
        RenderMesh2D(meshes["ground"], shaders["VertexColor"], modelMatrix);
        RenderMesh2D(meshes["grass"], shaders["VertexColor"], transform2D::Translate(0, grassHeight));

        // Draw moon
        complexObjects["moon"]->position = glm::vec3(100, SCREEN_H - 100, 0);
        RenderComplex("moon", deltaTimeSeconds);
    }

    // Draw UI
    {
        // Bullets
        complexObjects["bullet"]->position = glm::vec3(SCREEN_W - 25, SCREEN_H - 55, 0);
        for (int i = 0; i < bullets; ++i) RenderComplex("bullet", deltaTimeSeconds, transform2D::Translate(-50 * i, 0));
        // Hearts
        glm::vec3 heartPos(SCREEN_W - 25, SCREEN_H - 90, 0);
        for (int i = 0; i < hp; ++i) RenderMesh2D(meshes["heart"], shaders["VertexColor"], transform2D::Translate(heartPos.x - 50 * i, heartPos.y));
        // Player timer
        RenderMesh2D(meshes["timer_outline"], shaders["VertexColor"], transform2D::Translate(0, SCREEN_H - 7));
        RenderMesh2D(meshes["timer"], shaders["VertexColor"], transform2D::Translate(0, SCREEN_H - 7));
        RenderMesh2D(meshes["timer_cover"], shaders["VertexColor"], transform2D::Translate(SCREEN_W * 2 / 3, SCREEN_H - 7) * transform2D::Scale(1 - playerTimer / maxTimer, 1));

        // Game Startin /  Over UI
        if (status == -2) {
            complexObjects["gameOver"]->position = glm::vec3(SCREEN_W / 2, SCREEN_H / 2 + SCREEN_H / 8, 0);
            RenderComplex("gameOver", deltaTimeSeconds);
        }
        else if (status == -1) RenderComplex("startCountdown", deltaTimeSeconds);
    }


    RenderComplex("duck", deltaTimeSeconds, glm::mat3(1), redDuck, glm::vec3(1, 0.19f, 0.19f));
    RenderComplex("cursor", deltaTimeSeconds);
}


void Tema1::FrameEnd() {
}


void Tema1::OnInputUpdate(float deltaTime, int mods) {
}


void Tema1::OnKeyPress(int key, int mods) {
}


void Tema1::OnKeyRelease(int key, int mods) {
}


glm::vec3 Tema1::MouseToScreen(int mouseX, int mouseY) {
    glm::ivec2 resolution = window->GetResolution();
    return glm::vec3(mouseX * SCREEN_W / resolution.x, SCREEN_H - mouseY * SCREEN_H / resolution.y, 0);
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {
    // Update cursor position
    complexObjects["cursor"]->position = MouseToScreen(mouseX, mouseY);
}


void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) {
    if (status == -2) {
        ResetGame();
        return;
    }
    else if (status != 0 || button != GLFW_MOUSE_BUTTON_2) return;
    
    SetStatus(1);
}


void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) {
}


void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) {
}


void Tema1::OnWindowResize(int width, int height) {

}


void Tema1::ResetGame() {
    static_cast<tema1::Duck*>(complexObjects["duck"])->position.y = 0; // ascundem rata
    static_cast<tema1::StartCountdown*>(complexObjects["startCountdown"])->Start(3);

    status = -1;
    score = 0;
    hp = bullets = 3;

    shotTimer = 100;
    spawnTimer = 3;
}

void Tema1::SetStatus(int status) {
    auto* duck = static_cast<tema1::Duck*>(complexObjects["duck"]);
    duck->SetStatus(status, SCREEN_W, SCREEN_H);
    this->status = status;

    if (status == 0) {
        playerTimer = 6 - score / 5;
        if (playerTimer < 1) playerTimer = 1;
        maxTimer = playerTimer;
        duck->speed = tema1::Duck::baseSpeed * (1 + score / 10.0f);
    }
    else if (status == 1) {
        spawnTimer = 1.5f;
        shotTimer = 0;
        score++;
    }
    else if (status == 2) {
        hp--;
        spawnTimer = 1.5f;
    }
}

void Tema1::RenderComplex(std::string name, float deltaTime, glm::mat3 finalTransform, bool customColor, glm::vec3 color) {
    tema1::Complex* c = complexObjects[name];
    c->Update(deltaTime, SCREEN_W, SCREEN_H);
    if (!c->visible) return;

    auto parentMatrix = transform2D::Translate(c->position.x, c->position.y) * transform2D::Rotate(c->angle) * transform2D::Scale(c->scaleX, c->scaleY);
    for (auto& mesh : c->meshes) {
        if (!mesh.second.visible) continue;
        auto matrix = finalTransform * parentMatrix * mesh.second.modelMatrix;
        if (customColor) RenderMesh2D(meshes[mesh.first], matrix, color);
        else RenderMesh2D(meshes[mesh.first], shaders["VertexColor"], matrix);
    }
}
