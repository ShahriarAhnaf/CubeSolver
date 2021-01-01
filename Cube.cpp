#include <iostream>
#include <cmath>

using namespace std;

int cubo[6][3][3] = { 0 }; //the cube[face][rows][columns] top right start

string get_Color(int ask)
{
	string send;
	switch (ask)
	{	
	case 1: send = "W";
		break;
	case 2: send = "Y";
		break;
	case 3: send = "R";
		break;
	case 4: send = "O";
		break;
	case 5: send = "B";
		break;
	case 6: send = "G";
		break;
	}
	return send;
}

string get_Orient(int ask)
{
	string send;
	switch (ask)
	{
	case 1: send = "top";
		break;
	case 2: send = "bottom";
		break;
	case 3: send = "left";
		break;
	case 4: send = "right";
		break;
	case 5: send = "front";
		break;
	case 6: send = "back";
		break;
	}

	return send;
}

void MoveFace(int face) //moving clockwise
{
	int oldface[3][3] = { 0 };

	//initializing the array with the old faces
	//look into faster methods later
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			oldface[y][x] = cubo[face][y][x];
		}
	}

	//top row
	cubo[face][0][2] = oldface[2][2]; //bottom left is now top left
	cubo[face][0][1] = oldface[1][2]; //left middle is now top middle
	cubo[face][0][0] = oldface[0][2]; //top left is now top right
	//middle row
	cubo[face][1][0] = oldface[0][1]; //top middle is now middle right
	cubo[face][1][2] = oldface[2][1]; //bottom middle is now middle left
	//bottom row
	cubo[face][2][0] = oldface[0][0]; //top right is now bottom right
	cubo[face][2][1] = oldface[1][0]; //middle right is now bottom middle 
	cubo[face][2][2] = oldface[2][0]; //bottom right is now top left
}

void MoveFacePrime(int face)
{
	int oldface[3][3] = { 0 };

	//initializing the array with the old faces
	//look into faster methods later
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			oldface[x][y] = cubo[face][x][y];
		}
	}
}


void DisplayCube()
{
	
	for(int x = 0; x < 6; x++)cout << "face " << x+1 << "   ";

	cout << endl;
	for (int rows = 0; rows < 3; rows++)
	{
		for (int faces = 0; faces < 6; faces++)
		{
			for (int columns = 0; columns < 3; columns++)
			{
				cout << get_Color(cubo[faces][rows][columns]) << " ";
			}
			cout << "   ";
		}
		cout << endl;
	}
	cout << endl;
}
int main()
{
	int test = 0;
	enum Color { W = 1, Y, B, O, G, R };
	cout << "use the capital letter of the colour to represent that square i.e ( W = white)" << endl;
	cout << "1 = white face(top), 2 = yellow face(bottom), 3 = blue(front), 4= orange(right), 5 = green(back), 6= left(red)" << endl;

	cin >> test;
	if (test == 1) {
		for (int pp = 0; pp < 6; pp++) // getting input of the cube
		{
			cout << "enter face " << pp + 1 << ":" << endl;
			for (int zz = 0; zz < 3; zz++)
			{
				cout << "enter row from the right: " << endl;
				for (int z = 0; z < 3; z++) {
					char temp;
					Color c = W;
					cout << "here:";
					cin >> temp;
					switch (temp) // to take input from the user in form of letters
					{
					case 'W': c = W;
						break;
					case 'Y': c = Y;
						break;
					case 'R': c = R;
						break;
					case 'O': c = O;
						break;
					case 'B': c = B;
						break;
					case 'G': c = G;
						break;
					}
					cubo[pp][zz][z] = c;
				}
			}
		}
	}
	else
	{
		for (int pp = 0; pp < 6; pp++) // test input for fastest testing
		{
			
			for (int zz = 0; zz < 3; zz++)
			{
				for (int z = 0; z < 3; z++) 
				{
					cubo[pp][zz][z] = z+3;
				}
			}
		}
	}
	DisplayCube();
	MoveFace(0); // just testting
	DisplayCube();
}

