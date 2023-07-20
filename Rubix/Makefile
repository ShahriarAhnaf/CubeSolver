build: Cube.cpp Solver.cpp main.cpp
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver.out
debug: Cube.cpp Solver.cpp main.cpp
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver.out -g -DSTEP_THROUGH_DEBUGGER
run:build
	./CubeSolver.out