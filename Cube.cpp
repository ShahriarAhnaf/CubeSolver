#include <iostream>
#include <cmath>
#include "Cube.h"
using namespace std;


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

	while (true)
	{
		int face, move;
		cout << "enter face number and 1 for clockwise and 2 for counter clockwise, enter 7 for exit loop" << endl;
		cout << "face : "; 
		cin >> face;
		cout << endl;
		cout << "move direction : "; 
		cin >> move;
		cout << endl;
		
		 if (face == 7) break;
		if (move == 1) {
			MoveFace(face -1);
		}
		else
		{
			MoveFacePrime(face -1);	
		}

		DisplayCube();
	}

}

