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


class Corner // dynamic allocation pointers
{
	private: 
		char* cor[8*3];
	public:
		char* get_corner();
		void set_Corner(char* dynamic_edge);
};

class Edge // dynamic allocation pointers
{	
	private:
		char* ed[12*2];
	public:
	char* get_edge();
	void set_Edge(char* dynamic_edge);
};

// inherit from the homies
class RubixCube : public Edge, public Corner{
private:
	Corner corner;
	Edge edge;
public: 
	RubixCube(){
		char* dynamic_corner = new char[8*3]; //2d arrays are just chonks of 1d arrays. m * n length;
		char* dynamic_edge = new char[12*2]; //2d arrays are just chonks of 1d arrays. m * n length;
		set_Corner(dynamic_corner);
		set_Edge(dynamic_edge);
	};
	RubixCube(Corner &passed_Corner, Edge &passed_Edge){
		edge = passed_Edge;
		corner = passed_Corner;
	};
	//DESTROYYYYY
	~RubixCube(){
		char* corner = get_corner();
		char* edge = get_edge();
		delete[] corner;
		delete[] edge;
		corner = nullptr;
		edge = nullptr;
	};

	Corner get_Corner();
	Edge get_Edge();
	
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