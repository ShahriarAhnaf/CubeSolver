#include "CubeRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "../../Rubix/Inc/Cube.h"

// ImGui Includes
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Rubik's cube colors (Order MUST match COLOR enum in Cube.h: WHITE, BLUE, RED, GREEN, ORANGE, YELLOW)
glm::vec3 CUBE_COLORS[] = {
    glm::vec3(1.0f, 1.0f, 1.0f),  // WHITE
    glm::vec3(0.0f, 0.0f, 1.0f),  // BLUE
    glm::vec3(1.0f, 0.0f, 0.0f),  // RED
    glm::vec3(0.0f, 1.0f, 0.0f),  // GREEN
    glm::vec3(1.0f, 0.5f, 0.0f),  // ORANGE
    glm::vec3(1.0f, 1.0f, 0.0f)   // YELLOW
};

CubeRenderer::CubeRenderer() 
    : window(nullptr), windowWidth(800), windowHeight(600),
      shaderProgram(0), VAO(0), VBO(0), EBO(0),
      cameraPos(0.0f, 0.0f, 5.0f),
      cameraFront(0.0f, 0.0f, -1.0f),
      cameraUp(0.0f, 1.0f, 0.0f),
      yaw(-90.0f), pitch(0.0f),
      firstMouse(true), lastX(windowWidth/2.0f), lastY(windowHeight/2.0f),
      deltaTime(0.0f), lastFrame(0.0f)
{
    cubeState.model = glm::mat4(1.0f);
    cubeState.rotation = glm::vec3(0.0f);
    cubeState.scale = 1.0f;
    cubeState.isAnimating = false;
    cubeState.animationProgress = 0.0f;
    cubeState.targetRotation = glm::vec3(0.0f);
    cubeState.animationSpeed = 2.0f;
    
    faceRotation.isRotating = false;
    faceRotation.faceIndex = FACE_FRONT;
    faceRotation.currentAngle = 0.0f;
    faceRotation.targetAngle = 0.0f;
    faceRotation.rotationSpeed = 180.0f; // degrees per second

    // Initialize face colors using CUBE_COLORS based on standard mapping
    faceColors.resize(6);
    faceColors[FACE_UP] = CUBE_COLORS[WHITE];       // White
    faceColors[FACE_LEFT] = CUBE_COLORS[BLUE];      // Blue
    faceColors[FACE_FRONT] = CUBE_COLORS[RED];      // Red
    faceColors[FACE_RIGHT] = CUBE_COLORS[GREEN];    // Green
    faceColors[FACE_BACK] = CUBE_COLORS[ORANGE];   // Orange
    faceColors[FACE_BOTTOM] = CUBE_COLORS[YELLOW]; // Yellow
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
    glfwSetMouseButtonCallback(window, mouseButtonCallback); // Register mouse button callback

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
    initializeCubelets();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 330";
    // Pass 'false' to prevent ImGui from installing its own callbacks over ours
    ImGui_ImplGlfw_InitForOpenGL(window, false); 
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}

void CubeRenderer::render() {
    while (!glfwWindowShouldClose(window)) {
        // Process Input (already modified to check ImGui capture)
        processInput();
        updateAnimation();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- ImGui Window --- 
        bool geometryNeedsUpdate = false;
        ImGui::Begin("Cube Controls");
        
        // Spacing Slider
        if (ImGui::SliderFloat("Cubelet Spacing", &cubeletSpacing, 0.0f, 0.1f)) {
            // Clamp spacing to avoid negative cubelet size
            const float maxSpacing = 1.0f / 3.0f - 0.001f; // Max spacing before cubelet vanishes
            cubeletSpacing = glm::clamp(cubeletSpacing, 0.0f, maxSpacing);
            geometryNeedsUpdate = true; // Need to rebuild geometry if spacing changes
        }

        // Color Pickers
        ImGui::Separator();
        ImGui::Text("Face Colors:");
        const char* colorNames[] = {"White", "Blue", "Red", "Green", "Orange", "Yellow"};
        for (int i = 0; i < 6; ++i) {
            if (ImGui::ColorEdit3(colorNames[i], &CUBE_COLORS[i].x)) {
                geometryNeedsUpdate = true; // Need to rebuild geometry if colors change
                 // Update faceColors array as well, since it might be used elsewhere initially
                 // This assumes the standard face-to-color mapping
                 if (i == WHITE) faceColors[FACE_UP] = CUBE_COLORS[i];
                 else if (i == BLUE) faceColors[FACE_LEFT] = CUBE_COLORS[i];
                 else if (i == RED) faceColors[FACE_FRONT] = CUBE_COLORS[i];
                 else if (i == GREEN) faceColors[FACE_RIGHT] = CUBE_COLORS[i];
                 else if (i == ORANGE) faceColors[FACE_BACK] = CUBE_COLORS[i];
                 else if (i == YELLOW) faceColors[FACE_BOTTOM] = CUBE_COLORS[i];
            }
        }

        ImGui::End();
        // --- End ImGui Window ---

        // If spacing or colors changed, update the logical cube state colors and rebuild geometry
        if (geometryNeedsUpdate) {
             syncCubeletColors(); // This calls updateCubeletGeometry -> setupCubeGeometry
        }

        
        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use shader program for the cube
        glUseProgram(shaderProgram);
        
        // Update uniforms (View, Projection, Model for overall cube rotation)
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
            (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        
        // Update model matrix with rotation
        cubeState.model = glm::mat4(1.0f);
        cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeState.model = glm::rotate(cubeState.model, glm::radians(cubeState.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeState.model = glm::scale(cubeState.model, glm::vec3(cubeState.scale)); // Apply zoom scale
        
        // Apply face rotation if active (This part needs careful review)
        // The current setupCubeGeometry rebuilds the *entire* VBO in world space relative to cube center.
        // Applying another rotation here might double-rotate things or rotate around the wrong axis.
        // For now, comment out the direct face rotation matrix application here, as setupCubeGeometry 
        // positions cubelets based on logicalPos, which is updated by finalizeFaceRotation.
        /*
        if (faceRotation.isRotating) {
            glm::vec3 rotationAxis;
            // ... switch case for axis ...
            // This rotation should ideally be applied *per cubelet* based on their group,
            // not to the entire cubeState.model.
            // cubeState.model = glm::rotate(cubeState.model, glm::radians(faceRotation.currentAngle), rotationAxis);
        }
        */
        
        // Set uniforms
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
        
        // Check uniform locations... (error handling removed for brevity)
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeState.model)); // Pass the main model matrix
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        // Set light properties...
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        
        // Draw cubelets (using the potentially updated VBO)
        // We don't need drawCubelets anymore if setupCubeGeometry draws everything?
        // Let's bind the VAO and draw directly.
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the EBO
        
        // Calculate the total number of indices from visible faces
        unsigned int indexCount = 0;
        unsigned int baseVertex = 0; // Keep track of base vertex for glDrawElementsBaseVertex if needed
        for (const auto& cubelet : cubelets) {
            int visibleFaceCount = 0;
            for (int i = 0; i < 6; ++i) {
                if (cubelet.visibleFaces[i]) {
                    visibleFaceCount++;
                }
            }
            indexCount += visibleFaceCount * 6; // 6 indices per visible face
        }

        if (indexCount > 0) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); 
        }
        
        glBindVertexArray(0); 

        // Rendering ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void CubeRenderer::processInput() {
    ImGuiIO& io = ImGui::GetIO();
    // Only process game input if ImGui doesn't want the keyboard
    if (!io.WantCaptureKeyboard) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Camera movement
        const float cameraSpeed = 0.05f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        bool faceRotatedThisFrame = false; // Flag to check if a face rotation key was pressed
        // Number keys 1-6 for face rotation
        if (!faceRotation.isRotating) { // Only allow new face rotation if one isn't already happening
            for (int i = 0; i < 6; i++) {
                if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS) {
                     // Check if Shift is also pressed for counter-clockwise
                    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                        startFaceRotation(static_cast<FACE_ORIENTATION>(i), false); // Counter-clockwise
                    } else {
                        startFaceRotation(static_cast<FACE_ORIENTATION>(i), true);  // Clockwise rotation
                    }
                    faceRotatedThisFrame = true;
                    break; // Process only one key press per frame if multiple are held
                }
            }
        }

        // Cube rotation (only if no face rotation key was pressed this frame and no animation is ongoing)
        if (!faceRotatedThisFrame && !cubeState.isAnimating && !faceRotation.isRotating) {
            // Arrow keys for cube rotation
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                rotateCube(-1.0f, 0.0f);
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                rotateCube(1.0f, 0.0f);
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                rotateCube(0.0f, 1.0f);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                rotateCube(0.0f, -1.0f);
        }
    } // End of (!io.WantCaptureKeyboard) check
}

void CubeRenderer::rotateCube(float x, float y) {
    if (!cubeState.isAnimating) {
        cubeState.targetRotation = cubeState.rotation + glm::vec3(y * 90.0f, x * 90.0f, 0.0f);
        cubeState.isAnimating = true;
        cubeState.animationProgress = 0.0f;
    }
}

void CubeRenderer::rotateFace(FACE_ORIENTATION faceIndex, bool clockwise) {
    if (!faceRotation.isRotating) {
        startFaceRotation(faceIndex, clockwise);
    }
}

void CubeRenderer::startFaceRotation(FACE_ORIENTATION faceIndex, bool clockwise) {
    // Remove animation setup for now
    // if (!faceRotation.isRotating) { 
    //    faceRotation.isRotating = true;
    //    faceRotation.faceIndex = faceIndex;
    //    faceRotation.currentAngle = 0.0f;
    //    faceRotation.targetAngle = clockwise ? -90.0f : 90.0f; 
    // 
    //    // Get affected cubelets
    //    std::vector<int> rotatingIndices = getFaceCubeletIndices(faceIndex);
    //    for (int index : rotatingIndices) {
    //        cubelets[index].isRotating = true;
    //    }
    // }

    // Directly update the logical cube state and sync colors
    updateCubeState(faceIndex, clockwise);
    // syncCubeletColors(); // updateCubeState already calls syncCubeletColors

    std::cout << "DEBUG: Started Face Rotation: " << faceIndex << ", Clockwise: " << clockwise << std::endl;
}

void CubeRenderer::updateAnimation() {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Update cube rotation animation (mouse/arrow keys)
    if (cubeState.isAnimating) {
        cubeState.animationProgress += deltaTime * cubeState.animationSpeed;
        if (cubeState.animationProgress >= 1.0f) {
            cubeState.rotation = cubeState.targetRotation;
            cubeState.isAnimating = false;
        } else {
            cubeState.rotation = glm::mix(cubeState.rotation, cubeState.targetRotation, 
                                        cubeState.animationProgress);
        }
    }

    // --- REMOVE FACE ROTATION ANIMATION LOGIC --- 
    /*
    // Update face rotation animation
    if (faceRotation.isRotating) {
        float rotationStep = faceRotation.rotationSpeed * deltaTime;
        if (std::abs(faceRotation.currentAngle - faceRotation.targetAngle) <= rotationStep) {
            faceRotation.currentAngle = faceRotation.targetAngle;
            faceRotation.isRotating = false;
            // Finalize the face rotation (update logical positions)
            finalizeFaceRotation(faceRotation.faceIndex, faceRotation.targetAngle > 0);
            // Update the Rubik's Cube logic as well
            // updateCubeState(faceRotation.faceIndex, faceRotation.targetAngle > 0); // Called directly now
        } else {
            faceRotation.currentAngle += (faceRotation.targetAngle > 0 ? rotationStep : -rotationStep);
        }
        // Animate the face rotation
        // animateFaceRotation(faceRotation.faceIndex, faceRotation.currentAngle); // Defer animation
    }
    */
    // --- END REMOVED BLOCK ---
}

void CubeRenderer::cleanup() {
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

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
    
    const float baseCubeSize = 1.0f; // Base size of the whole 3x3x3 cube
    const float cubeletSize = baseCubeSize / 3.0f; // Size of one cubelet before spacing
    const float stickerOffset = 0.001f; // Slight offset for stickers to avoid z-fighting
    const float actualCubeSize = cubeletSize - cubeletSpacing; // Size of the visible cubelet face
    const float halfActualSize = actualCubeSize / 2.0f;

    // For each cubelet, add its geometry with the appropriate colors
    for (int i = 0; i < cubelets.size(); i++) {
        const Cubelet& cubelet = cubelets[i];
        
        // Center position of the cubelet, considering spacing
        float centerPosX = cubelet.logicalPos.x * cubeletSize;
        float centerPosY = cubelet.logicalPos.y * cubeletSize;
        float centerPosZ = cubelet.logicalPos.z * cubeletSize;
        
        // Calculate vertex positions relative to the cubelet center
        float r = centerPosX + halfActualSize;
        float l = centerPosX - halfActualSize;
        float u = centerPosY + halfActualSize;
        float d = centerPosY - halfActualSize;
        float f = centerPosZ + halfActualSize;
        float b = centerPosZ - halfActualSize;

        // Add faces with appropriate colors
        // Right face (+X)
        if (cubelet.visibleFaces[FACE_RIGHT]) {
            addFace(vertices, indices,
                r, d, b,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT],
                r, u, b,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT],
                r, u, f,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT],
                r, u, f,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT],
                r, d, f,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT],
                r, d, b,  1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_RIGHT]
            );
        }
        // Left face (-X)
        if (cubelet.visibleFaces[FACE_LEFT]) {
             addFace(vertices, indices,
                l, d, b, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT],
                l, d, f, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT],
                l, u, f, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT],
                l, u, f, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT],
                l, u, b, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT],
                l, d, b, -1.0f, 0.0f, 0.0f, cubelet.faceColors[FACE_LEFT]
            );
        }
        // Top face (+Y)
        if (cubelet.visibleFaces[FACE_UP]) {
            addFace(vertices, indices,
                l, u, b,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP],
                l, u, f,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP],
                r, u, f,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP],
                r, u, f,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP],
                r, u, b,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP],
                l, u, b,  0.0f, 1.0f, 0.0f, cubelet.faceColors[FACE_UP]
            );
        }
        // Bottom face (-Y)
        if (cubelet.visibleFaces[FACE_BOTTOM]) {
            addFace(vertices, indices,
                l, d, b,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM],
                r, d, b,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM],
                r, d, f,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM],
                r, d, f,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM],
                l, d, f,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM],
                l, d, b,  0.0f, -1.0f, 0.0f, cubelet.faceColors[FACE_BOTTOM]
            );
        }
        // Front face (+Z)
        if (cubelet.visibleFaces[FACE_FRONT]) {
            addFace(vertices, indices,
                l, d, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT],
                r, d, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT],
                r, u, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT],
                r, u, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT],
                l, u, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT],
                l, d, f,  0.0f, 0.0f, 1.0f, cubelet.faceColors[FACE_FRONT]
            );
        }
        // Back face (-Z)
        if (cubelet.visibleFaces[FACE_BACK]) {
            addFace(vertices, indices,
                l, d, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK],
                l, u, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK],
                r, u, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK],
                r, u, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK],
                r, d, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK],
                l, d, b,  0.0f, 0.0f, -1.0f, cubelet.faceColors[FACE_BACK]
            );
        }
    }
    
    // Create/update OpenGL buffers
    if (VAO == 0) glGenVertexArrays(1, &VAO);
    if (VBO == 0) glGenBuffers(1, &VBO);
    if (EBO == 0) glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Use GL_DYNAMIC_DRAW because spacing/colors can change frequently
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Indices might change if cubelets become invisible, so use DYNAMIC_DRAW here too? Or keep static?
    // Let's keep EBO static for now, assuming 27 cubelets always exist, just faces might be hidden.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); 
    
    // Vertex attribute pointers (remain the same)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float))); // Color
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO
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
    
    // Add indices for the triangles
    size_t baseIndex = vertices.size() / 9 - 6;  // Each vertex has 9 components
    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
    indices.push_back(baseIndex + 4);
    indices.push_back(baseIndex + 5);
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
    float zoomSpeed = 0.1f;
    float zoom = yoffset * zoomSpeed;
    cubeState.scale = glm::clamp(cubeState.scale + zoom, 0.1f, 2.0f);
}

void CubeRenderer::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void CubeRenderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    std::cout << "DEBUG: Mouse callback entered." << std::endl; // Check if it's called
    CubeRenderer* renderer = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(window));
    if (!renderer) {
         std::cerr << "ERROR: Renderer pointer is null in mouseCallback!" << std::endl;
         return;
    }
    std::cout << "DEBUG: Renderer pointer OK." << std::endl;

    // Now try accessing ImGui
    std::cout << "DEBUG: Accessing ImGui::GetIO()..." << std::endl;
    ImGuiIO& io = ImGui::GetIO(); // Is this the crashing line?
    std::cout << "DEBUG: ImGui::GetIO() OK." << std::endl;

    if (io.WantCaptureMouse) {
        std::cout << "DEBUG: ImGui wants mouse capture." << std::endl;
        ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
        std::cout << "DEBUG: ImGui_ImplGlfw_CursorPosCallback called." << std::endl;
        return;
    }
    std::cout << "DEBUG: ImGui does NOT want mouse capture." << std::endl;

    renderer->handleMouseMovement(xpos, ypos);
    std::cout << "DEBUG: Mouse callback exited normally." << std::endl;
}

void CubeRenderer::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        // Forward event to ImGui's handler WHEN it wants capture
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset); 
        return; // Don't process it in our application
    }

    // If ImGui doesn't want capture, process it normally
    CubeRenderer* renderer = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(window));
    if (renderer) {
        renderer->handleScroll(xoffset, yoffset);
    }
}

void CubeRenderer::updateCubeState(FACE_ORIENTATION faceIndex, bool clockwise) {
    switch (faceIndex) {
        case FACE_UP:
            clockwise ? cube.U(1) : cube.U_PRIME(1);
            break;
        case FACE_LEFT:
            clockwise ? cube.L(1) : cube.L_PRIME(1);
            break;
        case FACE_FRONT:
            clockwise ? cube.F(1) : cube.F_PRIME(1);
            break;
        case FACE_RIGHT:
            clockwise ? cube.R(1) : cube.R_PRIME(1);
            break;
        case FACE_BACK:
            clockwise ? cube.B(1) : cube.B_PRIME(1);
            break;
        case FACE_BOTTOM:
            clockwise ? cube.D(1) : cube.D_PRIME(1);
            break;
    }
    syncCubeletColors();
}

void CubeRenderer::updateFaceColors() {
    // Get the current vertex data
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    // Cube size (smaller for each piece)
    const float size = 0.3f;  // Smaller size for each piece
    const float halfSize = size / 2.0f;
    const float gap = 0.02f;  // Gap between pieces
    
    // Create 27 cubes (3x3x3)
    for (int x = -1; x <= 1; x++) {
        for (int y = -1; y <= 1; y++) {
            for (int z = -1; z <= 1; z++) {
                // Skip the center piece
                if (x == 0 && y == 0 && z == 0) continue;
                
                // Calculate position with gaps
                float posX = x * (size + gap);
                float posY = y * (size + gap);
                float posZ = z * (size + gap);
                
                // Get colors from the cube state
                glm::vec3 frontColor = (z == 1) ? faceColors[FACE_FRONT] : glm::vec3(0.2f);
                glm::vec3 backColor = (z == -1) ? faceColors[FACE_BACK] : glm::vec3(0.2f);
                glm::vec3 topColor = (y == 1) ? faceColors[FACE_UP] : glm::vec3(0.2f);
                glm::vec3 bottomColor = (y == -1) ? faceColors[FACE_BOTTOM] : glm::vec3(0.2f);
                glm::vec3 rightColor = (x == 1) ? faceColors[FACE_RIGHT] : glm::vec3(0.2f);
                glm::vec3 leftColor = (x == -1) ? faceColors[FACE_LEFT] : glm::vec3(0.2f);
                
                // Add faces with updated colors
                addFace(vertices, indices,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY - halfSize, posZ + halfSize,    0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 0.0f, 1.0f,   frontColor,
                    posX + halfSize, posY + halfSize, posZ + halfSize,     0.0f, 0.0f, 1.0f,   frontColor,
                    posX - halfSize, posY + halfSize, posZ + halfSize,    0.0f, 0.0f, 1.0f,   frontColor,
                    posX - halfSize, posY - halfSize, posZ + halfSize,   0.0f, 0.0f, 1.0f,   frontColor
                );
                
                // Add other faces similarly...
                // (Back, Top, Bottom, Right, Left faces)
            }
        }
    }
    
    // Update the VBO with new vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    // Update the EBO with new index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

int CubeRenderer::visualToLogicalFace(int visualFace) {
    // This mapping might need to change based on camera or cube orientation
    // Example: If visual front (0) corresponds to logical FRONT
    // This function might become unnecessary if inputs directly use FACE_ORIENTATION
    switch (visualFace) {
        case 0: return FACE_FRONT;  // Assuming visual 0 = Front
        case 1: return FACE_BACK;   // Assuming visual 1 = Back
        case 2: return FACE_LEFT;   // Assuming visual 2 = Left
        case 3: return FACE_RIGHT;  // Assuming visual 3 = Right
        case 4: return FACE_UP;     // Assuming visual 4 = Up
        case 5: return FACE_BOTTOM; // Assuming visual 5 = Bottom
        default: return -1; // Invalid
    }
}

int CubeRenderer::logicalToVisualFace(int logicalFace) {
    // Inverse of visualToLogicalFace
    // Might also become unnecessary
    switch (static_cast<FACE_ORIENTATION>(logicalFace)) {
        case FACE_FRONT: return 0;
        case FACE_BACK:  return 1;
        case FACE_LEFT:  return 2;
        case FACE_RIGHT: return 3;
        case FACE_UP:    return 4;
        case FACE_BOTTOM:return 5;
        default: return -1;
    }
}

void CubeRenderer::initializeCubelets() {
    cubelets.clear();
    cubelets.resize(27);
    int idx = 0;
    const float baseCubeSize = 1.0f; // Must match setupCubeGeometry
    const float cubeletSize = baseCubeSize / 3.0f;

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            for (int z = -1; z <= 1; ++z) {
                Cubelet& cubie = cubelets[idx]; // Get reference
                cubie.logicalPos = glm::ivec3(x, y, z);
                // Set initial model matrix based on logical position and CURRENT spacing
                cubie.modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z) * cubeletSize); 
                cubie.index = idx++;
                cubie.isRotating = false;
                
                // Determine which faces are visible (on the outside of the cube)
                cubie.visibleFaces[FACE_RIGHT] = (x == 1);
                cubie.visibleFaces[FACE_LEFT] = (x == -1);
                cubie.visibleFaces[FACE_UP] = (y == 1);
                cubie.visibleFaces[FACE_BOTTOM] = (y == -1);
                cubie.visibleFaces[FACE_FRONT] = (z == 1);
                cubie.visibleFaces[FACE_BACK] = (z == -1);
                
                // Set default colors (will be updated in syncCubeletColors)
                std::fill(cubie.faceColors.begin(), cubie.faceColors.end(), glm::vec3(0.2f));
            }
        }
    }
    
    // Sync cubelet colors with the Rubik's Cube state
    syncCubeletColors(); // This calls updateCubeletGeometry which calls setupCubeGeometry
}

bool CubeRenderer::isFaceVisible(const glm::ivec3& pos, FACE_ORIENTATION faceIndex) {
    switch (faceIndex) {
        case FACE_FRONT:  return pos.z == 1;
        case FACE_BACK:   return pos.z == -1;
        case FACE_LEFT:   return pos.x == -1;
        case FACE_RIGHT:  return pos.x == 1;
        case FACE_UP:     return pos.y == 1;
        case FACE_BOTTOM: return pos.y == -1;
        default:          return false;
    }
}

glm::vec3 CubeRenderer::colorFromLogical(COLOR logicalColor) {
    // New implementation using CUBE_COLORS array
    if (logicalColor >= WHITE && logicalColor <= YELLOW) {
        return CUBE_COLORS[logicalColor];
    }
    return glm::vec3(0.2f, 0.2f, 0.2f); // Default gray for invalid colors
}

void CubeRenderer::syncCubeletColors() {
    // Map logical faces to cubelet face indices (This seems unused, face var below is enough)
    // const int logicalToFace[6] = {
    //     FACE_UP, FACE_LEFT, FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_BOTTOM
    // };

    // For each cubelet, set the appropriate colors for visible faces
    for (auto& cubelet : cubelets) {
        const glm::ivec3& pos = cubelet.logicalPos;
        
        // Iterate through each potential face of the cubelet
        for (int face = 0; face < 6; ++face) { 
            FACE_ORIENTATION currentFace = static_cast<FACE_ORIENTATION>(face);

            // Set non-visible faces to gray
            if (!cubelet.visibleFaces[currentFace]) {
                cubelet.faceColors[currentFace] = glm::vec3(0.2f); // Gray for non-visible faces
                continue;
            }
            
            // For visible faces, determine the color from the Rubik's Cube state
            int faceIndex = -1; // The face of the RubixCube to query (FACE_UP, FACE_FRONT etc)
            int stickerIndex = -1; // The sticker index (0-8) on that face
            
            // Determine faceIndex and stickerIndex based on cubelet position and the face direction
            switch (currentFace) {
                case FACE_RIGHT: // +X face of the cubelet
                    faceIndex = FACE_RIGHT;
                    // Add specific check for center piece
                    if (pos.y == 0 && pos.z == 0) {
                        stickerIndex = 8; // CENTER
                    } else {
                        // Map Y and Z to sticker index (0-7) based on Cube.h diagram
                        // Row determined by Y (1 -> 0, 0 -> 1, -1 -> 2) => (1 - pos.y)
                        // Col determined by Z (-1 -> 0, 0 -> 1, 1 -> 2) => (1 + pos.z)
                        stickerIndex = (1 - pos.y) * 3 + (1 + pos.z);
                    }
                    break;
                case FACE_LEFT: // -X face of the cubelet
                    faceIndex = FACE_LEFT;
                    // Add specific check for center piece
                    if (pos.y == 0 && pos.z == 0) {
                         stickerIndex = 8; // CENTER
                    } else {
                        // Map Y and Z to sticker index (0-7)
                        // Row determined by Y (1 -> 0, 0 -> 1, -1 -> 2) => (1 - pos.y)
                        // Col determined by Z (1 -> 0, 0 -> 1, -1 -> 2) => (1 - pos.z)
                        stickerIndex = (1 - pos.y) * 3 + (1 - pos.z);
                    }
                    break;
                case FACE_UP: // +Y face of the cubelet
                    faceIndex = FACE_UP;
                    // Map Z and X to sticker index (0-8) based on Cube.h diagram and definitions
                    if (pos.z == -1) { // Top row (0, 1, 2)
                        stickerIndex = 1 + pos.x; // Maps x=-1 -> 0, x=0 -> 1, x=1 -> 2
                    } else if (pos.z == 0) { // Middle row (7, 8, 3)
                         if (pos.x == -1) stickerIndex = 7; // LEFT
                         else if (pos.x == 0) stickerIndex = 8; // CENTER
                         else stickerIndex = 3; // RIGHT
                    } else { // Bottom row (z=1) -> (6, 5, 4)
                         if (pos.x == -1) stickerIndex = 6; // BOTTOM_LEFT
                         else if (pos.x == 0) stickerIndex = 5; // BOTTOM
                         else stickerIndex = 4; // BOTTOM_RIGHT
                    }
                    break;
                case FACE_BOTTOM: // -Y face of the cubelet
                    faceIndex = FACE_BOTTOM;
                     // Map Z and X to sticker index (0-8)
                    if (pos.z == 1) { // Top row (0, 1, 2) - Note: Z is inverted view relative to UP face diagram
                        stickerIndex = 1 + pos.x; // Maps x=-1 -> 0, x=0 -> 1, x=1 -> 2
                    } else if (pos.z == 0) { // Middle row (7, 8, 3)
                         if (pos.x == -1) stickerIndex = 7; // LEFT
                         else if (pos.x == 0) stickerIndex = 8; // CENTER
                         else stickerIndex = 3; // RIGHT
                    } else { // Bottom row (z=-1) -> (6, 5, 4)
                         if (pos.x == -1) stickerIndex = 6; // BOTTOM_LEFT
                         else if (pos.x == 0) stickerIndex = 5; // BOTTOM
                         else stickerIndex = 4; // BOTTOM_RIGHT
                    }
                    break;
                case FACE_FRONT: // +Z face of the cubelet
                    faceIndex = FACE_FRONT;
                    // Map Y and X to sticker index (0-8)
                     if (pos.y == 1) { // Top row (0, 1, 2)
                        stickerIndex = 1 + pos.x; // Maps x=-1 -> 0, x=0 -> 1, x=1 -> 2
                    } else if (pos.y == 0) { // Middle row (7, 8, 3)
                         if (pos.x == -1) stickerIndex = 7; // LEFT
                         else if (pos.x == 0) stickerIndex = 8; // CENTER
                         else stickerIndex = 3; // RIGHT
                    } else { // Bottom row (y=-1) -> (6, 5, 4)
                         if (pos.x == -1) stickerIndex = 6; // BOTTOM_LEFT
                         else if (pos.x == 0) stickerIndex = 5; // BOTTOM
                         else stickerIndex = 4; // BOTTOM_RIGHT
                    }
                    break;
                 case FACE_BACK: // -Z face of the cubelet
                    faceIndex = FACE_BACK;
                     // Map Y and X to sticker index (0-8)
                     if (pos.y == 1) { // Top row (0, 1, 2) - Note: X is inverted view relative to FRONT face diagram
                        stickerIndex = 1 - pos.x; // Maps x=1 -> 0, x=0 -> 1, x=-1 -> 2
                    } else if (pos.y == 0) { // Middle row (7, 8, 3)
                         if (pos.x == 1) stickerIndex = 7; // LEFT (from back view)
                         else if (pos.x == 0) stickerIndex = 8; // CENTER
                         else stickerIndex = 3; // RIGHT (from back view)
                    } else { // Bottom row (y=-1) -> (6, 5, 4)
                         if (pos.x == 1) stickerIndex = 6; // BOTTOM_LEFT (from back view)
                         else if (pos.x == 0) stickerIndex = 5; // BOTTOM
                         else stickerIndex = 4; // BOTTOM_RIGHT (from back view)
                    }
                    break;
            }

            // Get the color from the logical cube state if mapping was successful
            if (faceIndex != -1 && stickerIndex != -1 && stickerIndex <= 8) { // stickerIndex 8 is center
                uint64_t faceValue = cube.get_face(faceIndex);
                COLOR color = static_cast<COLOR>(GET_COLOR(faceValue, stickerIndex));
                cubelet.faceColors[currentFace] = colorFromLogical(color);
            } else {
                // Fallback if mapping failed (should not happen for visible faces)
                cubelet.faceColors[currentFace] = glm::vec3(0.1f, 0.1f, 0.1f); // Dark Gray error color
                if(cubelet.visibleFaces[currentFace]) {
                    std::cerr << "Error: Failed to map visible cubelet face to sticker! Face: " << currentFace 
                              << ", Pos: (" << pos.x << "," << pos.y << "," << pos.z << ")" << std::endl;
                }
            }
        }
    }
    
    // Update the geometry with new colors
    updateCubeletGeometry();
}

void CubeRenderer::drawCubelets(const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // We need to set the main model matrix uniform here once
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeState.model));

    glBindVertexArray(VAO); // Bind the single VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the EBO
    
    // Calculate the total number of indices from visible faces
    unsigned int indexCount = 0;
    unsigned int baseVertex = 0; // Keep track of base vertex for glDrawElementsBaseVertex if needed
    for (const auto& cubelet : cubelets) {
        int visibleFaceCount = 0;
        for (int i = 0; i < 6; ++i) {
            if (cubelet.visibleFaces[i]) {
                visibleFaceCount++;
            }
        }
        indexCount += visibleFaceCount * 6; // 6 indices per visible face
    }

    if (indexCount > 0) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0); 
    }
    
    glBindVertexArray(0); 
}

void CubeRenderer::updateCubeletGeometry() {
    // Rebuild the entire VBO/EBO based on current cubelet state (colors) and spacing
    setupCubeGeometry(); 
}

// Remove or comment out animation helper functions for now
/*
void CubeRenderer::animateFaceRotation(FACE_ORIENTATION faceIndex, float angle) {
    // ... implementation ...
}

void CubeRenderer::finalizeFaceRotation(FACE_ORIENTATION faceIndex, bool clockwise) {
   // ... implementation ...
}
*/

std::vector<int> CubeRenderer::getFaceCubeletIndices(FACE_ORIENTATION faceIndex, int layer) {
    std::vector<int> indices;
    for (int i = 0; i < 27; ++i) {
        glm::ivec3 pos = cubelets[i].logicalPos;
        bool match = false;
        switch (faceIndex) {
            case FACE_UP:     match = (pos.y == 1); break;
            case FACE_LEFT:   match = (pos.x == -1); break;
            case FACE_FRONT:  match = (pos.z == 1); break;
            case FACE_RIGHT:  match = (pos.x == 1); break;
            case FACE_BACK:   match = (pos.z == -1); break;
            case FACE_BOTTOM: match = (pos.y == -1); break;
        }
        if (match) {
            indices.push_back(i);
        }
    }
    return indices;
}

// Implement the mouse button callback function
void CubeRenderer::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        // Forward event to ImGui's handler WHEN it wants capture
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        return; // Don't process it in our application
    }

    // If ImGui doesn't want capture, handle game-specific logic here (if any)
    // std::cout << "DEBUG: Mouse button event not captured by ImGui: " << button << ", " << action << std::endl;
    // CubeRenderer* renderer = static_cast<CubeRenderer*>(glfwGetWindowUserPointer(window));
    // if (renderer) {
    //    renderer->handleMouseButton(button, action, mods); // Example game handler
    // }
} 