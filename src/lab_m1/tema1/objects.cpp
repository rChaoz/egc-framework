#include "objects.h"
#include "lab_m1/lab3/transform2D.h"

#include <iostream>

Mesh* tema1::CreateRect(const std::string& name, glm::vec3 center, float width, float height, glm::vec3 color, bool fill, float zIndex) {
    float w = width / 2, h = height / 2;
    std::vector<VertexFormat> vertices =
    {
        VertexFormat(center + glm::vec3(-w, -h, zIndex), color),
        VertexFormat(center + glm::vec3(w, -h, zIndex), color),
        VertexFormat(center + glm::vec3(w , h, zIndex), color),
        VertexFormat(center + glm::vec3(-w, h, zIndex), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* tema1::CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill, float zIndex) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(glm::vec3(center.x, center.y, zIndex), color));
    if (fill) indices.push_back(0);

    const int NUM_POINTS = 50;
    for (int i = 0; i < NUM_POINTS; ++i) {
        double angle = 2 * M_PI * i / NUM_POINTS;
        vertices.push_back(VertexFormat(center + glm::vec3(cos(angle) * radius, sin(angle) * radius, zIndex), color));
        indices.push_back(i + 1);
    }

    if (fill) indices.push_back(1);

    Mesh* circle = new Mesh(name);
    circle->SetDrawMode(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* tema1::CreateTriangle(const std::string& name, glm::vec3 center, glm::vec3 p1Delta, glm::vec3 p2Delta, glm::vec3 p3Delta, glm::vec3 color, bool fill, float zIndex) {
    VertexFormat p1(glm::vec3(center.x + p1Delta.x, center.y + p1Delta.y, zIndex), color);
    VertexFormat p2(glm::vec3(center.x + p2Delta.x, center.y + p2Delta.y, zIndex), color);
    VertexFormat p3(glm::vec3(center.x + p3Delta.x, center.y + p3Delta.y, zIndex), color);

    std::vector<VertexFormat> vertices = { p1, p2, p3 };
    std::vector<unsigned int> indices = { 0, 1, 2 };

    Mesh* triangle = new Mesh(name);
    triangle->InitFromData(vertices, indices);
    if (!fill) triangle->SetDrawMode(GL_LINE_LOOP);

    return triangle;
}

// RECT
tema1::Rect::Rect() {
    this->top = this->right = this->bottom = this->left = 0;
    points = { glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,0,0) };
}

tema1::Rect::Rect(float top, float right, float bottom, float left) {
    this->top = top;
    this->right = right;
    this->bottom = bottom;
    this->left = left;
    points = { glm::vec3(-left, top, 0), glm::vec3(right, top, 0), glm::vec3(right, -bottom, 0), glm::vec3(-left, -bottom, 0) };
}

void tema1::Rect::Update(float angle, float scaleX, float scaleY) {
    auto matrix = transform2D::Rotate(angle) * transform2D::Scale(scaleX, scaleY);
    points = {
        matrix * glm::vec3(-left, top, 0),
        matrix * glm::vec3(-left, -bottom, 0),
        matrix * glm::vec3(right, -bottom, 0),
        matrix * glm::vec3(right, top, 0)
    };
}

int ccw(glm::vec2& A, glm::vec2& B, glm::vec2& C) {
    return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x);
}

bool intersect(glm::vec2& A, glm::vec2& B, glm::vec2& C, glm::vec2& D) {
    return ccw(A, C, D) != ccw(B, C, D) && ccw(A, B, C) != ccw(A, B, D);
}

bool tema1::Rect::ContainsPoint(glm::vec2 point) {
    // RayCast method
    float max = left;
    if (top > max) max = top;
    if (right > max) max = right;
    if (bottom > max) max = bottom;
    glm::vec2 start(-2 * max, 10);

    int count = 0;
    for (int i = 0; i < 4; ++i) {
        glm::vec2 a = points[i], b = points[i == 3 ? 0 : i + 1];
        if (intersect(a, b, start, point)) ++count;
    }

    return count % 2 == 1;
}

// COMPLEX MESH
tema1::ComplexMesh::ComplexMesh(glm::mat3 modelMatrix, bool visible) : modelMatrix(modelMatrix), visible(visible) {}

// COMPLEX
tema1::Complex::Complex(std::unordered_map<std::string, Mesh*>& worldMeshMap) : worldMeshMap(worldMeshMap) {
    visible = true;
    position = glm::vec3(0, 0, 0);
    angle = 0.0f;
    scaleX = scaleY = 1.0f;
}

tema1::Complex::~Complex() {
    for (auto mesh : meshes) {
        auto m = worldMeshMap[mesh.first];
        if (m) {
            delete m;
            worldMeshMap.erase(mesh.first);
        }
    }
}

bool tema1::Complex::PointInBox(glm::vec2 point) {
    box.Update(angle, scaleX, scaleY);
    return box.ContainsPoint(point - position);
}

void tema1::Complex::AddMesh(Mesh* mesh, glm::mat3 modelMatrix, bool visible) {
    auto id = mesh->GetMeshID();
    worldMeshMap[id] = mesh;
    AddMesh(id, modelMatrix);
}

void tema1::Complex::AddMesh(std::string id, glm::mat3 modelMatrix, bool visible) {
    meshes[id] = ComplexMesh(modelMatrix, visible);
}

void tema1::Complex::Update(float deltaTime, int screenW, int screenH) {}

// START COUNTDOWN
tema1::StartCountdown::StartCountdown(std::unordered_map<std::string, Mesh*>& worldMeshMap) : Complex(worldMeshMap) {
    timerTotal = 1;
    timer = 0;

    const glm::vec3 color(0.95f, 0.79f, 0.05f);
    AddMesh(CreateRect("startCountdown_1", glm::vec3(-80, 0, 0), 40, 150, color, true, 11));
    AddMesh(CreateRect("startCountdown_2", glm::vec3(0, 0, 0), 40, 150, color, true, 11));
    AddMesh(CreateRect("startCountdown_3", glm::vec3(80, 0, 0), 40, 150, color, true, 11));
}

void tema1::StartCountdown::Start(float timer) {
    timerTotal = timer;
    visible = true;
    this->timer = 0;
}

void tema1::StartCountdown::Update(float deltaTime, int screenW, int screenH) {
    if (timer > timerTotal) return;
    timer += deltaTime;
    if (timer > timerTotal) {
        visible = false;
        return;
    }
    position.x = screenW / 2;
    position.y = screenH / 2;

    float div = timerTotal / 3;
    meshes["startCountdown_2"].visible = timer < 2 * div;
    meshes["startCountdown_3"].visible = timer < div;
}

// SCORE
tema1::Score::Score(std::unordered_map<std::string, Mesh*>& worldMeshMap, int maxScore) : Complex(worldMeshMap) {
    score = highScore = 0;
    this->maxScore = maxScore;

    AddMesh(CreateRect("score", glm::vec3(-WIDTH / 2, 0, 0), WIDTH, HEIGHT, glm::vec3(1, 0.93f, 0.22f), true, 5.0f));
    AddMesh(CreateRect("score_cover", glm::vec3(-(WIDTH - 6) / 2, 0, 0), WIDTH - 6, HEIGHT - 6, glm::vec3(0), true, 5.5f));
    AddMesh(CreateRect("score_high", glm::vec3(0), 3, HEIGHT - 6, glm::vec3(.9f, .1f, .1f), true, 5.8f));
}
void tema1::Score::Update(float deltaTime, int screenW, int screenH) {
    position.x = screenW - MARGIN_RIGHT;
    position.y = screenH - MARGIN_TOP;

    float scorePercent = 1 - static_cast<float>(score) / MAX_SCORE;

    meshes["score_cover"].visible = score < MAX_SCORE;
    meshes["score_cover"].modelMatrix = transform2D::Translate(-3, 0) * transform2D::Scale(scorePercent, 1);
    meshes["score_high"].visible = highScore > 0;
    meshes["score_high"].modelMatrix = transform2D::Translate(-std::fmaxf(scorePercent, 0) * (WIDTH - 6) - 3, 0);
}

// TIMER
tema1::Timer::Timer(std::unordered_map<std::string, Mesh*>& worldMeshMap, int screenW): Complex(worldMeshMap) {
    cover = 0;

    glm::vec3 red(1, .1f, .1f), green(.1f, 1, .1f);
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-screenW / 6, -5, 10.2f), red),
        VertexFormat(glm::vec3(screenW / 6, -5, 10.2f), green),
        VertexFormat(glm::vec3(screenW / 6, 5, 10.2f), green),
        VertexFormat(glm::vec3(-screenW / 6, 5, 10.2f), red)
    };
    std::vector<unsigned int> indices{ 0, 1, 2, 2, 3, 0 };

    Mesh* timer = new Mesh("timer");
    timer->InitFromData(vertices, indices);
    AddMesh(timer);
    AddMesh(CreateRect("timer_cover", glm::vec3(-screenW / 6, 0, 0), screenW / 3, 10, glm::vec3(.1f), true, 10.5f));
    AddMesh(CreateRect("timer_outline", glm::vec3(0), screenW / 3 + 4, 14, glm::vec3(.1f), true, 10.0f));
}

void tema1::Timer::Update(float deltaTime, int screenW, int screenH) {
    position.x = screenW / 2;
    position.y = screenH - 7;
    // Scale cover
    meshes["timer_cover"].modelMatrix = transform2D::Translate(screenW / 6, 0)  * transform2D::Scale(cover, 1);
}

// BULLET
tema1::Bullet::Bullet(std::unordered_map<std::string, Mesh*>& worldMeshMap) : Complex(worldMeshMap) {
    timer = -1;

    AddMesh(CreateCircle("bullet_outer", glm::vec3(0), 30, glm::vec3(0.82f, 0.53f, 0.19f), true, 1.0f));
    AddMesh(CreateCircle("bullet_inner", glm::vec3(0), 20, glm::vec3(0.53f, 0.3f, 0.1f), true, 1.1f));
}

void tema1::Bullet::Shoot(glm::vec2 point) {
    position = point;
    this->timer = TIME_TO_HIT;
    visible = true;
    scaleX = scaleY = 1.0f;
}

void tema1::Bullet::Update(float deltaTime, int screenW, int screenH) {
    if (timer < 0) return;
    timer -= deltaTime;
    if (timer < 0) {
        visible = false;
        return;
    }
    scaleX = scaleY = (1 - SCALE_MIN) * (timer / TIME_TO_HIT) + SCALE_MIN;

}

// DUCK
tema1::Duck::Duck(std::unordered_map<std::string, Mesh*>& worldMeshMap) : Complex(worldMeshMap) {
    flapAngle = animationTimer = randomTimer = startingHeight = 0.0f;
    status = score = 0;

    box = tema1::Rect(65, 110, 65, 80);

    AddMesh(CreateTriangle("duck_body", glm::vec3(0, 0, 0),
        glm::vec3(-80, 25, 0), glm::vec3(-80, -25, 0), glm::vec3(80, 0, 0),
        glm::vec3(0.8f, 0.81f, 0.82f)));
    AddMesh(CreateTriangle("duck_left_wing", glm::vec3(-5, 5, 0),
        glm::vec3(-18, 0, 0), glm::vec3(18, 0, 0), glm::vec3(-7, 65, 0),
        glm::vec3(0.8f, 0.81f, 0.82f)));
    AddMesh(CreateTriangle("duck_right_wing", glm::vec3(-5, -5, 0),
        glm::vec3(-18, 0, 0), glm::vec3(18, 0, 0), glm::vec3(-7, -65, 0),
        glm::vec3(0.8f, 0.81f, 0.82f)));

    AddMesh(CreateCircle("duck_head", glm::vec3(70, 0, 0), 20, glm::vec3(0.95f, 0.96f, 0.96f), true, .5f));
    AddMesh(CreateCircle("duck_eye", glm::vec3(80, 0, 0), 3, glm::vec3(.0f, .0f, .0f), true, .7f));
    AddMesh(CreateTriangle("duck_beak", glm::vec3(100, 0, 0),
        glm::vec3(-10, 4, 0), glm::vec3(-10, -4, 0), glm::vec3(10, 0, 0),
        glm::vec3(0.87f, 0.49f, 0.1f)));
}

bool tema1::Duck::HeadingRight() {
    return angle < M_PI_2 || angle > M_PI + M_PI_2;
}

void tema1::Duck::SetStatus(int newStatus, int screenW, int screenH) {
    this->status = newStatus;
    if (status == 0) {
        angle = AI_DEG_TO_RAD(rand() % 30 + 30);
        if (rand() % 2 == 0) angle = M_PI - angle;
        position.x = rand() % static_cast<int>(screenW - box.left - box.right) + box.left;
        position.y = 0;
        if (rand() % 2 == 0) position.x = screenW;
    }
    else if (status == 1) {
        // Dying
        if (HeadingRight()) angle = AI_DEG_TO_RAD(80);
        else angle = AI_DEG_TO_RAD(100);
        animationTimer = 0;
        startingHeight = position.y;
    }
    else if (status == 2) {
        // Escaping
        angle = M_PI_2;
        animationTimer = 0;
        startingHeight = position.y;
    }
    else {
        position.y = 0;
    }
}

void tema1::Duck::Update(float deltaTime, int screenW, int screenH) {
    // Rata da din aripi
    float speed = baseSpeed * (1 + score / 10.0f);
    float flapSpeed = baseFlapSpeed * (1 + score / 20.0f);
    if (status == 1) flapSpeed *= 2.0f;
    else if (status == 2) flapSpeed *= 1.5f;

    flapAngle += deltaTime * flapSpeed;
    if (flapAngle > 2 * M_PI) flapAngle -= 2 * M_PI;
    float amount = sinf(this->flapAngle) * flapWideness;

    meshes["duck_right_wing"].modelMatrix = transform2D::Rotate(amount);
    meshes["duck_left_wing"].modelMatrix = transform2D::Rotate(-amount);

    if (status == 1) {
        animationTimer += deltaTime;
        float x = animationTimer * 1.2f;
        if (x > 1) return;
        // Dying animation
        float c1 = 1.70158f;
        float c3 = c1 + 1;

        float value = c3 * x * x * x - c1 * x * x;
        position.y = startingHeight - value * screenH;
        return;
    }
    else if (status == 2) {
        animationTimer += deltaTime;
        float x = animationTimer * .8f;
        if (x > 1) return;

        float value = x * x * x;
        position.y = startingHeight + value * screenH;
        return;
    }
    else if (status != 0) return;
    // Verificam daca rata iese din ecran
    float d = speed * deltaTime;
    position.x += cos(angle) * d;
    position.y += sin(angle) * d;

    bool changed = false;
    if (position.x - box.left < 0) {
        position.x = box.left;
        angle = M_PI - angle;
        changed = true;
    }
    else if (position.x + box.right > screenW) {
        position.x = screenW - box.right;
        angle = M_PI - angle;
        changed = true;
    }
    if (position.y - box.bottom < 0) {
        position.y = box.bottom;
        angle = -angle;
        changed = true;
    }
    else if (position.y + box.top > screenH) {
        position.y = screenH - box.top;
        angle = -angle;
        changed = true;
    }

    randomTimer -= deltaTime;
    if (!changed && randomTimer < 0) {
        randomTimer = .5f;

        if (rand() % 100 < score) {
            if (rand() % 2 == 0) angle = M_PI - angle;
            else angle = -angle;
        }
    }

    if (angle < 0) angle += 2 * M_PI;

    // Atunci cand rata se roteste 180, ochiul care era "sus" (deasupra ciocului) va ajunge "jos"
    if (HeadingRight()) meshes["duck_eye"].modelMatrix = transform2D::Translate(0, 5);
    else meshes["duck_eye"].modelMatrix = transform2D::Translate(0, -5);
}
