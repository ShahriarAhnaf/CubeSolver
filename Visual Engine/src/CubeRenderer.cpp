#include "CubeRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

// Rubik's cube colors
const glm::vec3 COLORS[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),  // Red
    glm::vec3(0.0f, 1.0f, 0.0f),  // Green
    glm::vec3(0.0f, 0.0f, 1.0f),  // Blue
    glm::vec3(1.0f, 1.0f, 0.0f),  // Yellow
    glm::vec3(1.0f, 0.5f, 0.0f),  // Orange
    glm::vec3(1.0f, 1.0f, 1.0f)   // White
};

CubeRenderer::CubeRenderer() 
    : window(nullptr), windowWidth(800), windowHeight(600),
      shaderProgram(0), VAO(0), VBO(0), EBO(0),
      cameraPos(0.0f, 0.0f, 5.0f),
      cameraFront(0.0f, 0.0f, -1.0f),
      cameraUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f), pitch(0.0f),
      firstMouse(true), lastX(windowWidth/2.0f), lastY(windowHeight/2.0f)
{
    cubeState.model = glm::mat4(1.0f);
    cubeState.rotation = glm::vec3(0.0f);
    cubeState.scale = 1.0f;
}

CubeRenderer::~CubeRenderer() {
    cleanup();
}

bool CubeRenderer::initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required for macOS

    // Create window
    window = glfwCreateWindow(windowWidth, windowHeight, "Rubik's Cube Visualizer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        cleanup();  // Clean up GLFW
        return false;
    }

    glfwMakeContextCurrent(window);
    
    // Set window user pointer to this instance
    glfwSetWindowUserPointer(window, this);
    
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Set up OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        cleanup();  // Clean up GLFW and window
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Compile shaders
    if (!compileShaders()) {
        cleanup();  // Clean up GLFW, window, and OpenGL context
        return false;
    }

    // Set up cube geometry
    setupCubeGeometry();
    setupCamera();

    return true;
}

void CubeRenderer::render() {
    while (!glfwWindowShouldClose(window)) {
        processInput();
        
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader program
        glUseProgram(shaderProgram);
        
        // Update uniforms
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
            (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        
        // Update model matrix with rotation
        cubeState.model = glm::mat4(1.0f);
        cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeState.model = glm::scale(cubeState.model, glm::vec3(cubeState.scale));
        
        // Set uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        if (modelLoc == -1 || viewLoc == -1 || projLoc == -1) {
            std::cerr << "Failed to get uniform locations" << std::endl;
        }
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeState.model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Set light properties
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        
        // Draw cube
        glBindVertexArray(VAO);
        // CAN BE OPTIMIZED TO DRAW ONLY THE FACES THAT ARE VISIBLE
        // Draw all vertices (26 pieces * 6 faces * 6 vertices per face)
        glDrawArrays(GL_TRIANGLES, 0, 26 * 6 * 6);
        glBindVertexArray(0);
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void CubeRenderer::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void CubeRenderer::cleanup() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
    if (shaderProgram != 0) glDeleteProgram(shaderProgram);
    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool CubeRenderer::compileShaders() {
    // Read shader files
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.open("shaders/cube.vert");
    fShaderFile.open("shaders/cube.frag");
    
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        return false;
    }

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // Compile shaders
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentCode);

    // Create shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return false;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

GLuint CubeRenderer::createShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
        return 0;
    }

    return shader;
}

void CubeRenderer::setupCubeGeometry() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Cube size (smaller for each piece)
    const float size = 0.3f;  // Smaller size for each piece
    const float halfSize = size / 2.0f;
    const float gap = 0.02f;  // Gap between pieces
    
    int pieceCount = 0;
    
    // Create 27 cubes (3x3x3)
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                // Skip the center piece
                if (x == 0 && y == 0 && z == 0) continue;
                
                pieceCount++;
                
                // Calculate position with gaps
                float posX = x * (size + gap);
                float posY = y * (size + gap);
                float posZ = z * (size + gap);
                
                // Determine colors for each face
                glm::vec3 frontColor = (z == 1) ? COLORS[0] : glm::vec3(0.2f);  // Red front
                glm::vec3 backColor = (z == -1) ? COLORS[4] : glm::vec3(0.2f);  // Orange back
                glm::vec3 topColor = (y == 1) ? COLORS[5] : glm::vec3(0.2f);    // White top
                glm::vec3 bottomColor = (y == -1) ? COLORS[3] : glm::vec3(0.2f); // Yellow bottom
                glm::vec3 rightColor = (x == 1) ? COLORS[2] : glm::vec3(0.2f);   // Blue right
                glm::vec3 leftColor = (x == -1) ? COLORS[1] : glm::vec3(0.2f);   // Green left
                
                // Front face
                addFace(vertices, indices,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY - halfSize, posZ + halfSize,    0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 0.0f, 1.0f,   frontColor,
                    posX - halfSize, posY + halfSize, posZ + halfSize,    0.0f, 0.0f, 1.0f,   frontColor,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   0.0f, 0.0f, 1.0f,   frontColor
                );
                
                // Back face
                addFace(vertices, indices,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  0.0f, 0.0f, -1.0f,  backColor,
                    posX - halfSize, posY + halfSize, posZ - halfSize,   0.0f, 0.0f, -1.0f,  backColor,
                    posX + halfSize, posY + halfSize, posZ - halfSize,    0.0f, 0.0f, -1.0f,  backColor,
                    posX + halfSize, posY + halfSize, posZ - halfSize,    0.0f, 0.0f, -1.0f,  backColor,
                    posX + halfSize, posY - halfSize, posZ - halfSize,   0.0f, 0.0f, -1.0f,  backColor,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  0.0f, 0.0f, -1.0f,  backColor
                );
                
                // Top face
                addFace(vertices, indices,
                    posX - halfSize, posY + halfSize, posZ - halfSize,   0.0f, 1.0f, 0.0f,   topColor,
                    posX + halfSize, posY + halfSize, posZ - halfSize,    0.0f, 1.0f, 0.0f,   topColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 1.0f, 0.0f,   topColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 1.0f, 0.0f,   topColor,
                    posX - halfSize, posY + halfSize, posZ + halfSize,    0.0f, 1.0f, 0.0f,   topColor,
                    posX - halfSize, posY + halfSize, posZ - halfSize,   0.0f, 1.0f, 0.0f,   topColor
                );
                
                // Bottom face
                addFace(vertices, indices,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  0.0f, -1.0f, 0.0f,  bottomColor,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   0.0f, -1.0f, 0.0f,  bottomColor,
                    posX + halfSize, posY - halfSize, posZ + halfSize,    0.0f, -1.0f, 0.0f,  bottomColor,
                    posX + halfSize, posY - halfSize, posZ + halfSize,    0.0f, -1.0f, 0.0f,  bottomColor,
                    posX + halfSize, posY - halfSize, posZ - halfSize,   0.0f, -1.0f, 0.0f,  bottomColor,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  0.0f, -1.0f, 0.0f,  bottomColor
                );
                
                // Right face
                addFace(vertices, indices,
                    posX + halfSize, posY - halfSize, posZ - halfSize,   1.0f, 0.0f, 0.0f,   rightColor,
                    posX + halfSize, posY + halfSize, posZ - halfSize,    1.0f, 0.0f, 0.0f,   rightColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     1.0f, 0.0f, 0.0f,   rightColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     1.0f, 0.0f, 0.0f,   rightColor,
                    posX + halfSize, posY - halfSize, posZ + halfSize,    1.0f, 0.0f, 0.0f,   rightColor,
                    posX + halfSize, posY - halfSize, posZ - halfSize,   1.0f, 0.0f, 0.0f,   rightColor
                );
                
                // Left face
                addFace(vertices, indices,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  -1.0f, 0.0f, 0.0f,  leftColor,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   -1.0f, 0.0f, 0.0f,  leftColor,
                    posX - halfSize, posY + halfSize, posZ + halfSize,    -1.0f, 0.0f, 0.0f,  leftColor,
                    posX - halfSize, posY + halfSize, posZ + halfSize,    -1.0f, 0.0f, 0.0f,  leftColor,
                    posX - halfSize, posY + halfSize, posZ - halfSize,   -1.0f, 0.0f, 0.0f,  leftColor,
                    posX - halfSize, posY - halfSize, posZ - halfSize,  -1.0f, 0.0f, 0.0f,  leftColor
                );
            }
        }
    }
    
    std::cout << "Generated " << pieceCount << " pieces" << std::endl;
    std::cout << "Total vertices: " << vertices.size() / 9 << std::endl;  // Each vertex has 9 components (3 pos, 3 normal, 3 color)
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void CubeRenderer::addFace(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                         float x1, float y1, float z1, float nx1, float ny1, float nz1, glm::vec3 color1,
                         float x2, float y2, float z2, float nx2, float ny2, float nz2, glm::vec3 color2,
                         float x3, float y3, float z3, float nx3, float ny3, float nz3, glm::vec3 color3,
                         float x4, float y4, float z4, float nx4, float ny4, float nz4, glm::vec3 color4,
                         float x5, float y5, float z5, float nx5, float ny5, float nz5, glm::vec3 color5,
                         float x6, float y6, float z6, float nx6, float ny6, float nz6, glm::vec3 color6) {
    // Add vertices for the first triangle
    vertices.push_back(x1); vertices.push_back(y1); vertices.push_back(z1);
    vertices.push_back(nx1); vertices.push_back(ny1); vertices.push_back(nz1);
    vertices.push_back(color1.r); vertices.push_back(color1.g); vertices.push_back(color1.b);
    
    vertices.push_back(x2); vertices.push_back(y2); vertices.push_back(z2);
    vertices.push_back(nx2); vertices.push_back(ny2); vertices.push_back(nz2);
    vertices.push_back(color2.r); vertices.push_back(color2.g); vertices.push_back(color2.b);
    
    vertices.push_back(x3); vertices.push_back(y3); vertices.push_back(z3);
    vertices.push_back(nx3); vertices.push_back(ny3); vertices.push_back(nz3);
    vertices.push_back(color3.r); vertices.push_back(color3.g); vertices.push_back(color3.b);
    
    // Add vertices for the second triangle
    vertices.push_back(x4); vertices.push_back(y4); vertices.push_back(z4);
    vertices.push_back(nx4); vertices.push_back(ny4); vertices.push_back(nz4);
    vertices.push_back(color4.r); vertices.push_back(color4.g); vertices.push_back(color4.b);
    
    vertices.push_back(x5); vertices.push_back(y5); vertices.push_back(z5);
    vertices.push_back(nx5); vertices.push_back(ny5); vertices.push_back(nz5);
    vertices.push_back(color5.r); vertices.push_back(color5.g); vertices.push_back(color5.b);
    
    vertices.push_back(x6); vertices.push_back(y6); vertices.push_back(z6);
    vertices.push_back(nx6); vertices.push_back(ny6); vertices.push_back(nz6);
    vertices.push_back(color6.r); vertices.push_back(color6.g); vertices.push_back(color6.b);
}

void CubeRenderer::setupCamera() {
    // Set up initial camera position and orientation
    cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);  // Moved further back to see the entire cube
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    
    // Initialize cube state
    cubeState.rotation = glm::vec3(0.0f);
    cubeState.scale = 1.0f;
}

void CubeRenderer::handleMouseMovement(double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.5f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    cubeState.rotation.y += xoffset;
    cubeState.rotation.x += yoffset;
    
    // Clamp vertical rotation
    if(cubeState.rotation.x > 89.0f)
        cubeState.rotation.x = 89.0f;
    if(cubeState.rotation.x < -89.0f)
        cubeState.rotation.x = -89.0f;
}

void CubeRenderer::handleScroll(double xoffset, double yoffset) {
    // Implement zoom functionality
}

void CubeRenderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void CubeRenderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    CubeRenderer* renderer = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->handleMouseMovement(xpos, ypos);
    }
}

void CubeRenderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    CubeRenderer* renderer = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->handleScroll(xoffset, yoffset);
    }
} 