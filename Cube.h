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
class dynamic2dArray{
	private:
	char* pointer;
	int row_limit;
	int column_limit;
	public:
	dynamic2dArray(int rows, int columns){
		pointer = new T[rows*columns];
		row_limit = rows;
		column_limit = columns;
	}
	~dynamic2dArray();
	char operator()(int rows, int columns){
		// error handle for index overflow
		// with array[2][5] -> array[10] -> 0-9 -> array[1][4]
		if (columns > column_limit || rows > row_limit){
			throw std::invalid_argument("out of index, array bounds must be within the 2d array");
		}
		// 
		return *((rows * column_limit * sizeof(char)) + columns*sizeof(char)); // return the derefed location of the array.
	}
};

class Corner : public dynamic2dArray<char> // dynamic allocation pointers
{
	private: 
		dynamic2dArray<char> cor(8,3);
	public:
		Corner(){
			dynamic2dArray(8,3);
		}
		~Corner();
		char* get_corner();
		void set_Corner(char* dynamic_edge);
};

class Edge : public dynamic2dArray<char> // dynamic allocation pointers
{	
	private:
		dynamic2dArray ed;
	public:
	// return a char pointer
	char* get_edge(){
		return ed; // dereference the char pointer to array for the array. 
	}
	void set_Edge(char* dynamic_edge){
	}
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