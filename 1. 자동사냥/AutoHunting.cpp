#include<iostream>
#include<algorithm>
#include<array>
#include<vector>
#include<functional>
#include<cmath>

using namespace std;

#ifdef _LOCAL
#include<sstream>
stringstream in("269 66	600	7	131 196 9 15	293 322 6 44	205 316 3 96	312 66 8 84	339 143 8 12	430 82 9 15	391 369 4 29");
ostream& out = cout;
#else
#include<fstream>
fstream in("input.txt");
fstream out("output.txt");
#endif

typedef unsigned int num;

struct Vector2 {
	num x, y;
	Vector2(num ax, num ay) :x(ax), y(ay) {};
	Vector2() :Vector2(0, 0) {};
};

struct Unit {
	Vector2 coord;
	num time;
	num exp;
	Unit(num ax, num ay, num atime, num aexp) : coord(ax, ay), time(atime), exp(aexp) {};
	Unit(num ax, num ay, num atime) : Unit(ax, ay, atime, 0) {};
	Unit() : Unit(0, 0, 0, 0) {};
	Unit(const Unit& obj) : Unit(obj.coord.x, obj.coord.y, obj.time, obj.exp) {};
};

typedef array<vector<Unit>, 8> Map;

static num EnemyToMap(int X, int Y);

void DoAutoHunting();

inline static num getWeight(const Unit& origin, const Unit& target) {
	return target.exp / (getLength(origin.coord, target.coord) + target.time);
	// Reward / Time (to get the reward)
}

inline static num getLength(const Vector2& origin, const Vector2& target) {
	return static_cast<num> (sqrt(pow(static_cast<double>(origin.x) - static_cast<double> (target.x), 2)
		+ pow(static_cast<double>(origin.y) - static_cast<double>(target.y), 2)));
}

Map EnemyMap;
array<int, 8> DirPriority;
size_t HighPriority;
Unit User;

int main() {
	num tempX, tempY, tempTime, tempExp;
	size_t uCase, counter;

	in >> tempX >> tempY >> tempTime;

	User = Unit(tempX, tempY, tempTime);

	in >> uCase;
	
	counter = 0;
	while (counter < uCase) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap[EnemyToMap(tempX, tempY)].push_back(Unit(tempX, tempY, tempTime, tempExp));
		counter++;
	}
	counter = 0;
	
	num Max = 0;
	while (counter < uCase) {
		DirPriority[counter] = EnemyMap[counter].size() + EnemyMap[(counter - 1) % 8].size() + EnemyMap[(counter + 1) % 8].size();
		if (Max < DirPriority[counter]) {
			HighPriority = counter;
			Max = DirPriority[counter];
		}
		counter++;
	}	
	counter = 0;


	return 0;
}

static num EnemyToMap(int X, int Y) {
	num whichMap = 0;
	if (static_cast<int> (User.coord.x) <= X) {
		if (static_cast<int> (User.coord.y) <= Y) {
			whichMap = 0;
		}
		else {
			whichMap = 6;
		}
	}
	else {
		if (static_cast<int> (User.coord.y) <= Y) {
			whichMap = 2;
		}
		else {
			whichMap = 4;
		}
	}

	switch (whichMap) {
	case 0:
		if (Y > X + static_cast<int> (User.coord.y) - static_cast<int> (User.coord.x)) {
			whichMap++;
		}
		break;
	case 4:
		if (Y < X + static_cast<int> (User.coord.y) - static_cast<int> (User.coord.x)) {
			whichMap++;
		}
		break;
	case 2:
		if (Y < (-X) + static_cast<int> (User.coord.y) + static_cast<int> (User.coord.x)) {
			whichMap++;
		}
		break;
	case 6:
		if (Y >(-X) + static_cast<int> (User.coord.y) + static_cast<int> (User.coord.x)) {
			whichMap++;
		}
		break;
	}
	return whichMap;
}