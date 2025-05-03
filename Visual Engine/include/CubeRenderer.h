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
    
    // Shader management
    bool compileShaders();
    GLuint createShader(GLenum type, const std::string& source);
    
    // Resource management
    void setupCubeGeometry();
    void setupCamera();
    
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