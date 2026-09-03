#include "CubeRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>

static const float CUBIE_SIZE = 0.47f;
static const float CUBIE_GAP  = 0.5f;  // center-to-center distance (slightly > CUBIE_SIZE for gaps)
static const float ANIM_SPEED = 360.0f; // degrees per second

CubeRenderer::CubeRenderer()
    : window(nullptr), windowWidth(900), windowHeight(700),
      shaderProgram(0), cubieVAO(0), cubieVBO(0),
      camDistance(6.0f), camYaw(35.0f), camPitch(25.0f),
      mouseDown(false), lastMouseX(0), lastMouseY(0),
      cubieCount(0), animating(false)
{
    memset(keyStates, 0, sizeof(keyStates));
    currentAnim = {};
    solver.on_stage = [this](const std::string& moves) {
        std::lock_guard<std::mutex> lock(stageMutex);
        stageMoves.push_back(moves);
    };
}

CubeRenderer::~CubeRenderer() {
    cleanup();
}

// Set up 26 cubies in solved state
void CubeRenderer::initCubies() {
    cubieCount = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                if (x == 0 && y == 0 && z == 0) continue; // no center cubie
                Cubie& c = cubies[cubieCount++];
                c.pos = glm::ivec3(x, y, z);
                for (int f = 0; f < 6; f++) c.faceColors[f] = COL_BLACK;

                // Assign sticker colors based on position
                if (x ==  1) c.faceColors[RFACE_RIGHT]  = COL_RED;
                if (x == -1) c.faceColors[RFACE_LEFT]   = COL_ORANGE;
                if (y ==  1) c.faceColors[RFACE_TOP]    = COL_WHITE;
                if (y == -1) c.faceColors[RFACE_BOTTOM] = COL_YELLOW;
                if (z ==  1) c.faceColors[RFACE_FRONT]  = COL_GREEN;
                if (z == -1) c.faceColors[RFACE_BACK]   = COL_BLUE;
            }
        }
    }
}

// Rebuild cubie colors from the RubixCube model state
void CubeRenderer::syncCubiesFromModel() {
    // Map from RubixCube sticker positions to 3D cubie positions
    // RubixCube face layout per face:
    //   0(TL) 1(T) 2(TR)
    //   7(L)       3(R)
    //   6(BL) 5(B) 4(BR)
    //
    // Solver FACE_ORIENTATION: UP=0, LEFT=1, FRONT=2, RIGHT=3, BACK=4, BOTTOM=5
    // Solver COLOR: WHITE=0, BLUE=1, RED=2, GREEN=3, ORANGE=4, YELLOW=5

    static const glm::vec3 colorMap[] = {
        COL_WHITE, COL_BLUE, COL_RED, COL_GREEN, COL_ORANGE, COL_YELLOW
    };

    // Reset all cubies to initial positions and black
    cubieCount = 0;
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                if (x == 0 && y == 0 && z == 0) continue;
                Cubie& c = cubies[cubieCount++];
                c.pos = glm::ivec3(x, y, z);
                for (int f = 0; f < 6; f++) c.faceColors[f] = COL_BLACK;
            }
        }
    }

    // Helper: find the cubie at a given position
    auto findCubie = [&](int x, int y, int z) -> Cubie* {
        for (int i = 0; i < cubieCount; i++)
            if (cubies[i].pos.x == x && cubies[i].pos.y == y && cubies[i].pos.z == z)
                return &cubies[i];
        return nullptr;
    };

    // Map face sticker index to (dx, dy) offset from face center in face-local coords
    // Index: 0=TL 1=T 2=TR 3=R 4=BR 5=B 6=BL 7=L
    static const int offsets[8][2] = {
        {-1,1},{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0}
    };

    // FACE_UP (y=+1): face-local X=cube X, face-local Y=cube -Z
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_UP), s);
        int cx = offsets[s][0], cz = -offsets[s][1];
        Cubie* c = findCubie(cx, 1, cz);
        if (c) c->faceColors[RFACE_TOP] = colorMap[color];
    }

    // FACE_BOTTOM (y=-1)
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_BOTTOM), s);
        int cx = offsets[s][0], cz = offsets[s][1];
        Cubie* c = findCubie(cx, -1, cz);
        if (c) c->faceColors[RFACE_BOTTOM] = colorMap[color];
    }

    // FACE_FRONT (z=+1)
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_FRONT), s);
        int cx = offsets[s][0], cy = offsets[s][1];
        Cubie* c = findCubie(cx, cy, 1);
        if (c) c->faceColors[RFACE_FRONT] = colorMap[color];
    }

    // FACE_BACK (z=-1)
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_BACK), s);
        int cx = -offsets[s][0], cy = offsets[s][1];
        Cubie* c = findCubie(cx, cy, -1);
        if (c) c->faceColors[RFACE_BACK] = colorMap[color];
    }

    // FACE_RIGHT (x=+1)
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_RIGHT), s);
        int cz = -offsets[s][0], cy = offsets[s][1];
        Cubie* c = findCubie(1, cy, cz);
        if (c) c->faceColors[RFACE_RIGHT] = colorMap[color];
    }

    // FACE_LEFT (x=-1)
    for (int s = 0; s < 8; s++) {
        int color = GET_COLOR(cubeModel.get_face(FACE_LEFT), s);
        int cz = offsets[s][0], cy = offsets[s][1];
        Cubie* c = findCubie(-1, cy, cz);
        if (c) c->faceColors[RFACE_LEFT] = colorMap[color];
    }
}

// Apply a visual move and also apply it to the RubixCube model
void CubeRenderer::applyMoveToModel(int axis, int layer, float direction) {
    if (axis == 0 && layer == 1 && direction > 0)      cubeModel.R(1);
    else if (axis == 0 && layer == 1 && direction < 0)  cubeModel.R_PRIME(1);
    else if (axis == 0 && layer == -1 && direction < 0) cubeModel.L(1);
    else if (axis == 0 && layer == -1 && direction > 0) cubeModel.L_PRIME(1);
    else if (axis == 1 && layer == 1 && direction > 0)  cubeModel.U(1);
    else if (axis == 1 && layer == 1 && direction < 0)  cubeModel.U_PRIME(1);
    else if (axis == 1 && layer == -1 && direction < 0) cubeModel.D(1);
    else if (axis == 1 && layer == -1 && direction > 0) cubeModel.D_PRIME(1);
    else if (axis == 2 && layer == 1 && direction > 0)  cubeModel.F(1);
    else if (axis == 2 && layer == 1 && direction < 0)  cubeModel.F_PRIME(1);
    else if (axis == 2 && layer == -1 && direction < 0) cubeModel.B(1);
    else if (axis == 2 && layer == -1 && direction > 0) cubeModel.B_PRIME(1);
}

// Parse a move string like "R U' F2" and enqueue each as animated moves
void CubeRenderer::enqueueMoveString(const std::string& moveStr) {
    std::istringstream iss(moveStr);
    std::string tok;
    while (iss >> tok) {
        int axis = -1, layer = 0;
        float dir = 1.0f;
        int count = 1;

        char base = tok[0];
        bool prime = tok.find('\'') != std::string::npos;
        bool dbl = tok.find('2') != std::string::npos;

        if (base == 'R')      { axis = 0; layer =  1; dir =  1.0f; }
        else if (base == 'L') { axis = 0; layer = -1; dir = -1.0f; }
        else if (base == 'U') { axis = 1; layer =  1; dir =  1.0f; }
        else if (base == 'D') { axis = 1; layer = -1; dir = -1.0f; }
        else if (base == 'F') { axis = 2; layer =  1; dir =  1.0f; }
        else if (base == 'B') { axis = 2; layer = -1; dir = -1.0f; }
        else continue;

        if (prime) dir = -dir;
        if (dbl) count = 2;

        for (int i = 0; i < count; i++)
            enqueueMove(axis, layer, dir);
    }
}

void CubeRenderer::enqueueMove(int axis, int layer, float direction) {
    MoveAnim m;
    m.axis = axis;
    m.layer = layer;
    m.direction = direction;
    m.angle = 0.0f;
    m.speed = ANIM_SPEED;
    moveQueue.push(m);
}

// After animation completes, permute cubie positions and rotate their sticker colors
void CubeRenderer::commitMove(const MoveAnim& anim) {
    // Build the same rotation matrix used for animation (90 degrees)
    glm::vec3 axisVec(0.0f);
    axisVec[anim.axis] = 1.0f;
    glm::mat3 rot = glm::mat3(glm::rotate(glm::mat4(1.0f),
                               glm::radians(90.0f * anim.direction), axisVec));

    // The 6 face normals, indexed by Face enum
    static const glm::vec3 faceNormals[6] = {
        { 1, 0, 0}, // RFACE_RIGHT
        {-1, 0, 0}, // RFACE_LEFT
        { 0, 1, 0}, // RFACE_TOP
        { 0,-1, 0}, // RFACE_BOTTOM
        { 0, 0, 1}, // RFACE_FRONT
        { 0, 0,-1}, // RFACE_BACK
    };

    for (int i = 0; i < cubieCount; i++) {
        int coord = cubies[i].pos[anim.axis];
        if (coord != anim.layer) continue;

        // Rotate position
        glm::vec3 newPos = rot * glm::vec3(cubies[i].pos);
        cubies[i].pos = glm::ivec3(glm::round(newPos));

        // Rotate sticker colors: figure out where each face normal ends up
        glm::vec3 oldColors[6];
        for (int f = 0; f < 6; f++) oldColors[f] = cubies[i].faceColors[f];

        for (int f = 0; f < 6; f++) {
            glm::vec3 rotatedNormal = rot * faceNormals[f];
            // Find which face slot this normal now matches
            for (int g = 0; g < 6; g++) {
                if (glm::dot(rotatedNormal, faceNormals[g]) > 0.9f) {
                    cubies[i].faceColors[g] = oldColors[f];
                    break;
                }
            }
        }
    }

    applyMoveToModel(anim.axis, anim.layer, anim.direction);
}

void CubeRenderer::addCubieFace(std::vector<float>& verts,
    glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
    glm::vec3 normal, glm::vec3 color)
{
    // Two triangles: p0-p1-p2, p0-p2-p3
    auto push = [&](glm::vec3 pos) {
        verts.push_back(pos.x); verts.push_back(pos.y); verts.push_back(pos.z);
        verts.push_back(normal.x); verts.push_back(normal.y); verts.push_back(normal.z);
        verts.push_back(color.r); verts.push_back(color.g); verts.push_back(color.b);
    };
    push(p0); push(p1); push(p2);
    push(p0); push(p2); push(p3);
}

void CubeRenderer::buildVertices(std::vector<float>& verts) {
    verts.clear();
    float h = CUBIE_SIZE * 0.5f;

    glm::mat4 animRot(1.0f);
    int animLayer = 9999;
    int animAxis = -1;
    if (animating) {
        animAxis = currentAnim.axis;
        animLayer = currentAnim.layer;
        glm::vec3 axisVec(0.0f);
        axisVec[animAxis] = 1.0f;
        animRot = glm::rotate(glm::mat4(1.0f),
                              glm::radians(currentAnim.angle * currentAnim.direction),
                              axisVec);
    }

    for (int i = 0; i < cubieCount; i++) {
        const Cubie& c = cubies[i];
        glm::vec3 center = glm::vec3(c.pos) * CUBIE_GAP;

        // Check if this cubie is in the animating layer
        bool inAnimLayer = animating &&
            ((animAxis == 0 && c.pos.x == animLayer) ||
             (animAxis == 1 && c.pos.y == animLayer) ||
             (animAxis == 2 && c.pos.z == animLayer));

        // Face definitions: {normal axis, sign, corner offsets}
        struct FaceDef {
            glm::vec3 normal;
            glm::vec3 corners[4];
            int colorIdx;
        };
        FaceDef faces[6] = {
            // Right (+X)
            { {1,0,0}, {{h,-h,h},{h,h,h},{h,h,-h},{h,-h,-h}}, RFACE_RIGHT },
            { {-1,0,0}, {{-h,-h,-h},{-h,h,-h},{-h,h,h},{-h,-h,h}}, RFACE_LEFT },
            { {0,1,0}, {{-h,h,h},{h,h,h},{h,h,-h},{-h,h,-h}}, RFACE_TOP },
            { {0,-1,0}, {{-h,-h,-h},{h,-h,-h},{h,-h,h},{-h,-h,h}}, RFACE_BOTTOM },
            { {0,0,1}, {{-h,-h,h},{h,-h,h},{h,h,h},{-h,h,h}}, RFACE_FRONT },
            { {0,0,-1}, {{h,-h,-h},{-h,-h,-h},{-h,h,-h},{h,h,-h}}, RFACE_BACK },
        };

        for (int f = 0; f < 6; f++) {
            glm::vec3 p[4];
            glm::vec3 n = faces[f].normal;

            for (int v = 0; v < 4; v++) {
                p[v] = center + faces[f].corners[v];
                if (inAnimLayer) {
                    p[v] = glm::vec3(animRot * glm::vec4(p[v], 1.0f));
                }
            }
            if (inAnimLayer) {
                n = glm::vec3(animRot * glm::vec4(n, 0.0f));
            }

            addCubieFace(verts, p[0], p[1], p[2], p[3], n, c.faceColors[faces[f].colorIdx]);
        }
    }
}

// ---- OpenGL setup and render ----

bool CubeRenderer::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(windowWidth, windowHeight, "Rubik's Cube", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        cleanup();
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    if (!compileShaders()) {
        cleanup();
        return false;
    }

    // Create VAO/VBO for dynamic cubie geometry
    glGenVertexArrays(1, &cubieVAO);
    glGenBuffers(1, &cubieVBO);

    initCubies();
    return true;
}

void CubeRenderer::render() {
    float lastFrame = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        float dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput();

        // Pull finished solver stages into the animation queue
        {
            std::lock_guard<std::mutex> lock(stageMutex);
            for (const std::string& m : stageMoves) enqueueMoveString(m);
            stageMoves.clear();
        }
        if (solving() && solveFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            std::string solution = solveFuture.get();
            if (solution.empty()) std::cout << "No solution found within depth limit" << std::endl;
            else std::cout << "Solution: " << solution << std::endl;
        }

        // Advance animation
        if (!animating && !moveQueue.empty()) {
            currentAnim = moveQueue.front();
            moveQueue.pop();
            animating = true;
        }
        if (animating) {
            currentAnim.angle += currentAnim.speed * dt;
            if (currentAnim.angle >= 90.0f) {
                currentAnim.angle = 90.0f;
                commitMove(currentAnim);
                animating = false;
            }
        }

        // Build geometry
        std::vector<float> verts;
        buildVertices(verts);

        glBindVertexArray(cubieVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubieVBO);
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);

        // Configure vertex attributes once; VAO stores this state.
        static bool attributesConfigured = false;
        if (!attributesConfigured) {
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            attributesConfigured = true;
        }

        // Camera: orbit around origin
        float camX = camDistance * cosf(glm::radians(camPitch)) * cosf(glm::radians(camYaw));
        float camY = camDistance * sinf(glm::radians(camPitch));
        float camZ = camDistance * cosf(glm::radians(camPitch)) * sinf(glm::radians(camYaw));
        glm::vec3 eye(camX, camY, camZ);

        glm::mat4 view = glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        glm::mat4 model(1.0f);

        glClearColor(0.15f, 0.15f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::vec3 lightPos(4.0f, 6.0f, 5.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(eye));

        int vertexCount = (int)(verts.size() / 9);
        glBindVertexArray(cubieVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void CubeRenderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Moves: R, L, U, D, F, B -- hold Shift for prime (counter-clockwise)
    auto tryMove = [&](int key, int axis, int layer, float cwDir) {
        bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
        if (pressed && !keyStates[key] && !solving()) {
            bool shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                          glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
            enqueueMove(axis, layer, shift ? -cwDir : cwDir);
        }
        keyStates[key] = pressed;
    };

    //        key          axis  layer  cw-direction
    tryMove(GLFW_KEY_R,    0,     1,    1.0f);
    tryMove(GLFW_KEY_L,    0,    -1,   -1.0f);
    tryMove(GLFW_KEY_U,    1,     1,    1.0f);
    tryMove(GLFW_KEY_D,    1,    -1,   -1.0f);
    tryMove(GLFW_KEY_F,    2,     1,    1.0f);
    tryMove(GLFW_KEY_B,    2,    -1,   -1.0f);

    // Space: solve the cube
    {
        bool pressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        if (pressed && !keyStates[GLFW_KEY_SPACE] && !animating && moveQueue.empty() && !solving()) {
            RubixCube copy = cubeModel;
            solveFuture = std::async(std::launch::async,
                                     [this, copy]() mutable { return solver.Solve_Cube(copy, 8); });
        }
        keyStates[GLFW_KEY_SPACE] = pressed;
    }

    // M: scramble with 5 random moves
    {
        bool pressed = glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS;
        if (pressed && !keyStates[GLFW_KEY_M] && !animating && moveQueue.empty() && !solving()) {
            const char* moves[] = {"R","R'","L","L'","U","U'","D","D'","F","F'","B","B'"};
            static std::mt19937 rng(std::random_device{}());
            std::uniform_int_distribution<int> dist(0, 11);
            std::string scramble;
            for (int i = 0; i < 5; i++) {
                if (i > 0) scramble += " ";
                scramble += moves[dist(rng)];
            }
            std::cout << "Scramble: " << scramble << std::endl;
            enqueueMoveString(scramble);
        }
        keyStates[GLFW_KEY_M] = pressed;
    }
}

void CubeRenderer::cleanup() {
    if (cubieVAO) glDeleteVertexArrays(1, &cubieVAO);
    if (cubieVBO) glDeleteBuffers(1, &cubieVBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool CubeRenderer::compileShaders() {
    std::string vertexCode, fragmentCode;
    std::ifstream vFile("shaders/cube.vert"), fFile("shaders/cube.frag");

    if (!vFile.is_open() || !fFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        return false;
    }

    std::stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();
    vertexCode = vStream.str();
    fragmentCode = fStream.str();

    GLuint vert = createShader(GL_VERTEX_SHADER, vertexCode);
    GLuint frag = createShader(GL_FRAGMENT_SHADER, fragmentCode);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "Shader link error: " << log << std::endl;
        return false;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return true;
}

GLuint CubeRenderer::createShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

// ---- GLFW callbacks ----

void CubeRenderer::framebufferSizeCallback(GLFWwindow* w, int width, int height) {
    glViewport(0, 0, width, height);
    auto* r = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(w));
    if (r) { r->windowWidth = width; r->windowHeight = height; }
}

void CubeRenderer::mouseButtonCallback(GLFWwindow* w, int button, int action, int /*mods*/) {
    auto* r = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(w));
    if (!r) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        r->mouseDown = (action == GLFW_PRESS);
        if (r->mouseDown)
            glfwGetCursorPos(w, &r->lastMouseX, &r->lastMouseY);
    }
}

void CubeRenderer::cursorPosCallback(GLFWwindow* w, double x, double y) {
    auto* r = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(w));
    if (!r || !r->mouseDown) return;

    float dx = (float)(x - r->lastMouseX);
    float dy = (float)(y - r->lastMouseY);
    r->lastMouseX = x;
    r->lastMouseY = y;

    r->camYaw   += dx * 0.3f;
    r->camPitch += dy * 0.3f;
    if (r->camPitch >  89.0f) r->camPitch =  89.0f;
    if (r->camPitch < -89.0f) r->camPitch = -89.0f;
}

void CubeRenderer::scrollCallback(GLFWwindow* w, double /*xoff*/, double yoff) {
    auto* r = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(w));
    if (!r) return;
    r->camDistance -= (float)yoff * 0.5f;
    if (r->camDistance < 2.0f) r->camDistance = 2.0f;
    if (r->camDistance > 20.0f) r->camDistance = 20.0f;
}
