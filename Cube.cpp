#include <iostream>
#include <cmath>

using namespace std;


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

int main()
{

	int cubo[6][3][3] = {0}; //the cube[face][rows][columns] top right start

	enum Color{W=1,Y, R, O, B, G};
	cout << "use the capital letter of the colour to represent that square i.e ( W = white)" << endl;
	cout << "1 = white face(top), 2 = yellow face(bottom), 3 = blue(front), 4= orange(right), 5 = green(back), 6= left(red)" << endl;
	
	for (int pp = 0; pp < 6; pp++) // getting input of the cube
	{
		cout << "enter face " << pp + 1 << ":"<< endl;
		for (int zz = 0; zz < 3; zz++)
		{
			cout << "enter row from the right: " << endl;
			for (int z = 0; z < 3; z++) {
				char temp;
				Color c;
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

	
		for (int pp = 0; pp < 6; pp++) // outputting the layout cube
		{
			cout << endl<< get_Orient(pp+1) << " face" << endl; 
			for (int zz = 0; zz < 3; zz++)
			{
				for (int z = 0; z < 3; z++) 
				
				{
					cout << get_Color(cubo[pp][zz][z]) << " ";
				}
				cout << endl;
			}
		}
}

