#pragma once
#ifndef CUBE_H
#define CUBE_H
//ROYGBV
enum colors{
	R=0,
	O,
	Y,
	G,
	B,
	V
};

enum goal{
	cross=1,
	first_layer,
	middle_layer,
	final_layer
};


class Cube_face{
	private:
		Cube_face();
		Cube_face();
		~Cube_face();
		char face[3][3];
	private:
};
class RubixCube{
private:
	Cube_face* front;
	Cube_face* back;
	Cube_face* left;
	Cube_face* right;
	Cube_face* top;
	Cube_face* bottom;
public: 
	RubixCube();
	~RubixCube();
};

class GameState{

};
#endif