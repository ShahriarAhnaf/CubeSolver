build: Cube.cpp Solver.cpp main.cpp
	g++ -std=c++11 Cube.cpp Solver.cpp main.cpp -lncurses -o CubeSolver
run::build
	./CubeSolver