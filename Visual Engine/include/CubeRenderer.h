#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <queue>
#include <future>
#include <mutex>

#include "Cube.h"
#include "Solver.h"

// Standard Rubik's cube colors
static const glm::vec3 COL_WHITE  (1.0f, 1.0f, 1.0f);
static const glm::vec3 COL_YELLOW (1.0f, 1.0f, 0.0f);
static const glm::vec3 COL_RED    (0.8f, 0.0f, 0.0f);
static const glm::vec3 COL_ORANGE (1.0f, 0.5f, 0.0f);
static const glm::vec3 COL_BLUE   (0.0f, 0.0f, 0.8f);
static const glm::vec3 COL_GREEN  (0.0f, 0.6f, 0.0f);
static const glm::vec3 COL_BLACK  (0.08f, 0.08f, 0.08f);

enum RenderFace { RFACE_RIGHT=0, RFACE_LEFT, RFACE_TOP, RFACE_BOTTOM, RFACE_FRONT, RFACE_BACK };

struct Cubie {
    glm::ivec3 pos;           // logical position (-1,0,1) per axis
    glm::vec3 faceColors[6];  // indexed by Face enum, COL_BLACK if internal
};

struct MoveAnim {
    int axis;       // 0=X(R/L), 1=Y(U/D), 2=Z(F/B)
    int layer;      // which slice: -1, 0, or 1
    float direction; // +1 or -1 (clockwise vs counter-clockwise from positive axis)
    float angle;    // current animation angle (0 -> 90)
    float speed;    // degrees per second
};

class CubeRenderer {
public:
    CubeRenderer();
    ~CubeRenderer();

    bool initialize();
    
    // Main render loop
    void render();
    void cleanup();

private:
    GLFWwindow* window;
    int windowWidth, windowHeight;

    GLuint shaderProgram;
    GLuint cubieVAO, cubieVBO;

    // Camera: orbits around origin
    float camDistance;
    float camYaw, camPitch;
    bool mouseDown;
    double lastMouseX, lastMouseY;

    // 26 visible cubies
    Cubie cubies[26];
    int cubieCount;

    // Animation
    MoveAnim currentAnim;
    bool animating;
    std::queue<MoveAnim> moveQueue;

    // Key debounce
    bool keyStates[GLFW_KEY_LAST + 1];

    // Cube logic state
    RubixCube cubeModel;
    Solver solver;

    // Solve runs on a worker thread; finished stages land in stageMoves for the render loop.
    std::future<std::string> solveFuture;
    std::mutex stageMutex;
    std::vector<std::string> stageMoves;
    bool solving() const { return solveFuture.valid(); }

    bool showNet;  // print the text net after each move (T)

    void initCubies();
    void syncCubiesFromModel();
    void enqueueMove(int axis, int layer, float direction);
    void enqueueMoveString(const std::string& moveStr);
    void applyMoveToModel(int axis, int layer, float direction);
    void commitMove(const MoveAnim& anim);

    // Build per-frame geometry from cubie state
    void buildVertices(std::vector<float>& verts);
    void addCubieFace(std::vector<float>& verts,
                      glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3,
                      glm::vec3 normal, glm::vec3 color);

    bool compileShaders();
    GLuint createShader(GLenum type, const std::string& source);
    void processInput();

    static void framebufferSizeCallback(GLFWwindow* w, int width, int height);
    static void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* w, double x, double y);
    static void scrollCallback(GLFWwindow* w, double xoff, double yoff);
};
