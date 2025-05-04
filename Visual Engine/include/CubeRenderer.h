#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <memory>

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
    } cubeState;
    
    // Sphere state
    struct SphereState {
        glm::mat4 model;
        glm::vec3 rotation;
        float scale;
    } sphereState;
    
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
    
    // Mouse state
    bool firstMouse;
    float lastX;
    float lastY;
}; 