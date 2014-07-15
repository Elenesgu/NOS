#include<iostream>
#include<algorithm>
#include<list>
#include<array>
#include<vector>

using namespace std;



#ifdef _LOCAL
#include<sstream>
stringstream in("269 66	600	7	131 196 9 15	293 322 6 44	205 316 3 96	312 66 8 84	339 143 8 12	430 82 9 15	391 369 4 29");
#else
istream& in = cin;
#endif

typedef unsigned int num;

struct Vector2 {
	num x, y;
	Vector2() :x(0), y(0) {};
	Vector2(num ax, num ay) :x(ax), y(ay) {};
};

struct Unit {
	Vector2 coord;
	num time;
	num exp;
	Unit(const Unit& obj) : coord(obj.coord.x, obj.coord.y), time(obj.time), exp(obj.exp) {};
	Unit(num ax, num ay, num atime, num aexp) : coord(ax, ay), time(atime), exp(aexp) {};
	Unit(num ax, num ay, num atime) : Unit(ax, ay, atime, 0) {};
	Unit() : Unit(0, 0, 0, 0) {};
};

typedef array<vector<Unit>, 8> Map;

num EnemyToMap(int X, int Y);

Map EnemyMap;
Unit User;

int main() {
	num tempX, tempY, tempTime, tempExp;
	size_t uCase;

	in >> tempX >> tempY >> tempTime;

	User = Unit(tempX, tempY, tempTime);

	in >> uCase;

	while (uCase--) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap[EnemyToMap(tempX, tempY)].push_back(Unit(tempX, tempY, tempTime, tempExp));
	}

	return 0;
}

num EnemyToMap(int X, int Y) {
	num whichMap = 0;
	if (User.coord.x <= X) {
		if (User.coord.y <= Y) {
			whichMap = 0;
		}
		else {
			whichMap = 6;
		}
	}
	else {
		if (User.coord.y <= Y) {
			whichMap = 2;
		}
		else {
			whichMap = 4;
		}
	}

	switch (whichMap) {
	case 0:
		if (Y > X + User.coord.y - User.coord.x) {
			whichMap++;
		}
		break;
	case 4:
		if (Y < X + User.coord.y - User.coord.x) {
			whichMap++;
		}
		break;
	case 2:
		if (Y < (-X) + User.coord.y + User.coord.x) {
			whichMap++;
		}
		break;
	case 6:
		if (Y > (-X) + User.coord.y + User.coord.x) {
			whichMap++;
		}
		break;
	}
	return whichMap;
}