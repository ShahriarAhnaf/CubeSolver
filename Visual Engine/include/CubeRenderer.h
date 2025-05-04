#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <memory>
#include <array>
#include "Cube.h"

class CubeRenderer {
public:
    CubeRenderer();
    ~CubeRenderer();

    // Initialize OpenGL context and resources
    bool initialize();
    
    // Main render loop
    void render();
    
    // Handle window events
    void processInput();
    
    // Cleanup resources
    void cleanup();

private:
    // Window management
    GLFWwindow* window;
    int windowWidth;
    int windowHeight;
    
    // OpenGL resources
    GLuint shaderProgram;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    
    // Camera properties
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float yaw;
    float pitch;
    
    // Cube state
    struct CubeState {
        glm::mat4 model;
        glm::vec3 rotation;
        float scale;
        
        // Animation state
        bool isAnimating;
        float animationProgress;
        glm::vec3 targetRotation;
        float animationSpeed;
    } cubeState;
    
    // Sphere state
    struct SphereState {
        glm::mat4 model;
        glm::vec3 rotation;
        float scale;
    } sphereState;
    
    // Face rotation state
    struct FaceRotation {
        bool isRotating;
        FACE_ORIENTATION faceIndex;
        float currentAngle;
        float targetAngle;
        float rotationSpeed;
    } faceRotation;
    
    // Animation timing
    float deltaTime;
    float lastFrame;
    float cubeletSpacing = 0.02f; // Added: Spacing between cubelets
    
    // Shader management
    bool compileShaders();
    GLuint createShader(GLenum type, const std::string& source);
    
    // Resource management
    void setupCubeGeometry();
    void setupSphereGeometry();
    void setupCamera();
    void addFace(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                float x1, float y1, float z1, float nx1, float ny1, float nz1, glm::vec3 color1,
                float x2, float y2, float z2, float nx2, float ny2, float nz2, glm::vec3 color2,
                float x3, float y3, float z3, float nx3, float ny3, float nz3, glm::vec3 color3,
                float x4, float y4, float z4, float nx4, float ny4, float nz4, glm::vec3 color4,
                float x5, float y5, float z5, float nx5, float ny5, float nz5, glm::vec3 color5,
                float x6, float y6, float z6, float nx6, float ny6, float nz6, glm::vec3 color6);
    
    // Input handling
    void handleMouseMovement(double xpos, double ypos);
    void handleScroll(double xoffset, double yoffset);
    
    // Static callback functions for GLFW
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    // Mouse state
    bool firstMouse;
    float lastX;
    float lastY;
    
    // Cube control methods
    void rotateCube(float x, float y);
    void rotateFace(FACE_ORIENTATION faceIndex, bool clockwise);
    void updateAnimation();
    void startFaceRotation(FACE_ORIENTATION faceIndex, bool clockwise);
    
    // Rubik's Cube state
    RubixCube cube;
    
    // Face colors mapping
    std::vector<glm::vec3> faceColors;
    
    // Update cube state after face rotation
    void updateCubeState(FACE_ORIENTATION faceIndex, bool clockwise);
    void updateFaceColors();
    
    // Convert between visual and logical face indices
    int visualToLogicalFace(int visualFace);
    int logicalToVisualFace(int logicalFace);

    // Per-cubelet representation
    struct Cubelet {
        glm::ivec3 logicalPos; // Logical position in the 3x3x3 grid
        glm::mat4 modelMatrix; // Model matrix for this cubelet
        int index; // Unique index for identification
        bool isRotating = false;
        
        // Store colors for each face (right, left, up, down, front, back)
        std::array<glm::vec3, 6> faceColors;
        
        // Is this face visible (on the outside of the cube)?
        std::array<bool, 6> visibleFaces;

        void applyRotation(const glm::mat4& rotation) {
            modelMatrix = rotation * modelMatrix;
        }
        
        void resetRotation() {
            const float spacing = 0.32f;
            modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(logicalPos) * spacing);
        }
    };
    std::vector<Cubelet> cubelets; // 27 cubelets

    // Helper functions for cubelet management
    void initializeCubelets();
    void drawCubelets(const glm::mat4& view, const glm::mat4& projection);
    void animateFaceRotation(FACE_ORIENTATION faceIndex, float angle);
    std::vector<int> getFaceCubeletIndices(FACE_ORIENTATION faceIndex, int layer = 0);
    void finalizeFaceRotation(FACE_ORIENTATION faceIndex, bool clockwise);
    
    // Color management
    void syncCubeletColors();
    void updateCubeletGeometry();
    glm::vec3 colorFromLogical(COLOR logicalColor);
    bool isFaceVisible(const glm::ivec3& pos, FACE_ORIENTATION faceIndex);
    
    // Draw a single cubelet
    void drawCubelet(const Cubelet& cubelet, const glm::mat4& view, const glm::mat4& projection);
}; 