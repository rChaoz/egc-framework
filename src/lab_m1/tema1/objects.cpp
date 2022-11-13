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

    std::cout << "start(" << start.x << ", " << start.y << ")  ";
    std::cout << "point(" << point.x << ", " << point.y << ")  " << std::endl;
    int count = 0;
    for (int i = 0; i < 4; ++i) {
        glm::vec2 a = points[i], b = points[i == 3 ? 0 : i + 1];
        std::cout << "a(" << a.x << ", " << a.y << ")  ";
        std::cout << "b(" << a.x << ", " << a.y << ")  " << std::endl;
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
    AddMesh(CreateRect("startCountdown_1", glm::vec3(-80, 0, 0), 40, 150, color, true, 15));
    AddMesh(CreateRect("startCountdown_2", glm::vec3(0, 0, 0), 40, 150, color, true, 15));
    AddMesh(CreateRect("startCountdown_3", glm::vec3(80, 0, 0), 40, 150, color, true, 15));
}

void tema1::StartCountdown::Start(float timer) {
    timerTotal = timer;
    this->timer = 0;
}

void tema1::StartCountdown::Update(float deltaTime, int screenW, int screenH) {
    if (timer > timerTotal) return;
    timer += deltaTime;
    position.x = screenW / 2;
    position.y = screenH / 2;

    float div = timerTotal / 3;
    meshes["startCountdown_2"].visible = timer < 2 * div;
    meshes["startCountdown_3"].visible = timer < div;
}

// DUCK
tema1::Duck::Duck(std::unordered_map<std::string, Mesh*>& worldMeshMap) : Complex(worldMeshMap) {
    speed = baseSpeed;
    flapAngle = animationTimer = startingHeight = 0.0f;
    status = 0;

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
    flapAngle += deltaTime * speed / (status == 0 ? 20 : status == 1 ? 10 : 15);
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

    if (position.x - box.left < 0) {
        position.x = box.left;
        angle = M_PI - angle;
    }
    else if (position.x + box.right > screenW) {
        position.x = screenW - box.right;
        angle = M_PI - angle;
    }
    if (position.y - box.bottom < 0) {
        position.y = box.bottom;
        angle = -angle;
    }
    else if (position.y + box.top > screenH) {
        position.y = screenH - box.top;
        angle = -angle;
    }
    if (angle < 0) angle += 2 * M_PI;

    // Atunci cand rata se roteste 180, ochiul care era "sus" (deasupra ciocului) va ajunge "jos"
    if (HeadingRight()) meshes["duck_eye"].modelMatrix = transform2D::Translate(0, 5);
    else meshes["duck_eye"].modelMatrix = transform2D::Translate(0, -5);
}