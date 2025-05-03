#include "CubeRenderer.h"
#include <iostream>

int main() {
    CubeRenderer renderer;
    
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }
    
    renderer.render();
    
    return 0;
} 