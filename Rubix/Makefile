build: Cube.cpp Solver.cpp main.cpp
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver.out
debug: Cube.cpp Solver.cpp main.cpp
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver.out -g 
run:build
	./CubeSolver.out
step_run:
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver.out -DSTEP_THROUGH_DEBUGGER && ./CubeSolver.out