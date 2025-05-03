# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror
LDFLAGS = -lncurses

# Directories
SRC_DIR = src
INC_DIR = Inc
BUILD_DIR = build
TEST_DIR = Testing

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Main executable
MAIN = CubeSolver.out
TEST_EXE = Test_Cube.out

# Include paths
INC_PARAMS = $(addprefix -I,$(INC_DIR))

# Default target
all: $(BUILD_DIR) $(MAIN)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_PARAMS) -c $< -o $@

# Link main executable
$(MAIN): $(OBJS) main.o
	$(CXX) $(OBJS) main.o -o $@ $(LDFLAGS)

# Compile main.cpp
main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INC_PARAMS) -c $< -o $@

# Debug build
debug: CXXFLAGS += -g
debug: all

# Run the program
run: all
	./$(MAIN)

# Step through debugger
step_through: CXXFLAGS += -g -DSTEP_THROUGH_DEBUGGER
step_through: all
	./$(MAIN)

# Test build
test: $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INC_PARAMS) $(TEST_DIR)/test_pass_by_ref.cpp -o $(TEST_EXE)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(MAIN) $(TEST_EXE) *.o

.PHONY: all debug run step_through test clean