src = src/*
INC_DIRS = Inc/
INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d) #for each INC_DIR add a -I
CFLAGS = -std=c++11 -lncurses -Wall -Werror


build: main.cpp
	g++ $(CFLAGS) $(INC_PARAMS) $(src) main.cpp -o CubeSolver.out
debug: main.cpp
	g++ -g $(CFLAGS) $(INC_PARAMS) $(src) main.cpp -o CubeSolver.out  
run:build
	./CubeSolver.out
step_through_run:
	g++ $(CFLAGS) $(INC_PARAMS) $(src) main.cpp  -o CubeSolver.out -DSTEP_THROUGH_DEBUGGER && ./CubeSolver.out
test: 
	g++ -g $(CFLAGS) $(INC_PARAMS) $(src) Testing/test_pass_by_ref.cpp -o Test_Cube.out