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


struct corner // dynamic allocation pointers
{
	char* cor[8*3];
};

struct edge // dynamic allocation pointers
{
	char* ed[12*2];
};

class RubixCube{
private:
	corner Corner;
	edge Edge;
public: 
	RubixCube(){
		char* dynamic_corner = new char[8*3]; //2d arrays are just chonks of 1d arrays. m * n length;
		char* dynamic_edge = new char[12*2]; //2d arrays are just chonks of 1d arrays. m * n length;
		set_Corner(dynamic_corner);
		set_Edge(dynamic_edge);
	};
	RubixCube(corner &passed_Corner, edge &passed_Edge){
		Edge = passed_Edge;
		Corner = passed_Corner;
	};
	//DESTROYYYYY
	~RubixCube(){
		char* corner = get_Corner()->cor;
		char* edge = get_Edge()->ed;
	};

	corner* get_Corner();
	void set_Corner(char* dynamic_corner);
	edge* get_Edge();
	void set_Edge(char* dynamic_edge);
	};

class GameState{
private: 
	RubixCube Cube;
	bool closer;
public: 
	GameState();
	~GameState();
};
#endif