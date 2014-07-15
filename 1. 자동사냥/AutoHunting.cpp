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
	Unit(num ax, num ay, num atime, num aexp) : coord(ax, ay), time(atime), exp(aexp) {};
	Unit(num ax, num ay, num atime) : Unit(ax, ay, atime) {};
	Unit() : Unit(0, 0, 0, 0) {};
};

typedef array<vector<Unit>, 8> Map;


Map EnemyMap;


int main() {
	num tempX, tempY, tempTime, tempExp;
	size_t uCase;

	in >> tempX >> tempY >> tempTime;

	Unit User(tempX, tempY, tempTime);

	in >> uCase;

	while (uCase--) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap[0].push_back(Unit(tempX, tempY, tempTime, tempExp));
	}

	return 0;
}