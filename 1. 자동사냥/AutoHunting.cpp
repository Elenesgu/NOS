#include<iostream>
#include<algorithm>
#include<array>
#include<vector>
#include<string>
#include<functional>
#include<cmath>
#include<fstream>

using namespace std;

#ifdef _LOCAL
fstream in("Input.txt");
ostream& out = cout;
#else
fstream in("Input.txt");
fstream out(Output.txt");
#endif

typedef unsigned int num;

const char CRLF = '\n';

struct Vector2 {
	num x, y;
	Vector2(num ax, num ay) :x(ax), y(ay) {};
	Vector2() :Vector2(0, 0) {};
	Vector2(const Vector2& obj) : Vector2(obj.x, obj.y) {};
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

Map EnemyMap;
array<num, 8> DirPriority;
size_t HighPriority;
Unit User;
vector<Unit> PriorityMap;
string UserLog;
num doTime;
num Monsternum = 0;

bool HuntEnd = false;

auto VecAdd = [](Unit& obj) {PriorityMap.push_back(obj); };

static num EnemyToMap(int X, int Y);

inline static double getLength(const Vector2& origin, const Vector2& target) {
	return sqrt(pow(static_cast<double>(origin.x) - static_cast<double> (target.x), 2)
		+ pow(static_cast<double>(origin.y) - static_cast<double>(target.y), 2));
}

inline static double getWeight(const Unit& origin, const Unit& target, double Length) {
	return static_cast<double> (target.exp) / (Length + static_cast<double> (target.time)) ;
	// Reward / Time (to get the reward)
}

inline static double getWeight(const Unit& origin, const Unit& target) {
	return getWeight(origin, target, (getLength(origin.coord, target.coord)));
	// Reward / Time (to get the reward)
}

inline static void PrintLog(const Vector2& obj) {
	UserLog += to_string(obj.x);
	UserLog += " ";
	UserLog += to_string(obj.y);
	UserLog += CRLF;
}

static Unit& FindTarget();
static void Hunt(Unit& enemy);
static num doTest();

int main() {
	num tempX, tempY, tempTime, tempExp;
	size_t uCase, counter;
#ifdef _LOCAL
	if (!in) {
		out << "No File" << endl;
		return 0;
	}
#endif

	in >> tempX >> tempY >> doTime;

	User = Unit(tempX, tempY, doTime);

	in >> uCase;
	
	counter = 0;
	while (counter < uCase) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap[EnemyToMap(tempX, tempY)].push_back(Unit(tempX, tempY, tempTime, tempExp));
		counter++;
	}
	counter = 0;
	
	num Max = 0;
	while (counter < 8) {
		DirPriority[counter] = EnemyMap[counter].size() + EnemyMap[(counter - 1) % 8].size() + EnemyMap[(counter + 1) % 8].size();
		if (Max < DirPriority[counter]) {
			HighPriority = counter;
			Max = DirPriority[counter];
		}
		counter++;
	}
	PriorityMap.reserve(Max * 3);

	for_each(EnemyMap[HighPriority].begin(), EnemyMap[HighPriority].end(), VecAdd);
	for_each(EnemyMap[(HighPriority + 1)%8].begin(), EnemyMap[(HighPriority + 1)%8].end(), VecAdd);
	for_each(EnemyMap[(HighPriority - 1)%8].begin(), EnemyMap[(HighPriority - 1)%8].end(), VecAdd);

	auto result = doTest();
	out << doTime - User.time << endl << User.exp << endl;

	out << result << endl << UserLog;

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

static void Hunt(Unit& enemy) {
	if (HuntEnd) {
		return;
	}
	PrintLog(enemy.coord);
	User.exp += enemy.exp;
	enemy.exp = 0;
	User.time -= enemy.time;
	User.time -= static_cast<int> ( getLength(User.coord, enemy.coord) );
	User.coord = enemy.coord;
	Monsternum++;
}

static Unit& FindTarget() {
	double minWeight = 0, tmp;

	Unit* target = nullptr;
	for (auto itr = PriorityMap.begin(); itr != PriorityMap.end(); itr++) {
		tmp = getWeight(User, (*itr));
		if (minWeight < tmp) {
			if (getLength(User.coord, (*itr).coord) + (*itr).exp < User.time) {
				minWeight = tmp;
				target = &(*itr);
			}
		}
	}
	if (target == nullptr){
		HuntEnd = true;
	}
	return *target;
}

static num doTest() {
	bool Expand = true;
	while (User.time > 0 && !HuntEnd) {
		Hunt(FindTarget());
		if (HuntEnd && Expand) {
			for (int i = 2; i < 7; i++) {
				for_each(EnemyMap[(HighPriority + i) % 8].begin(), EnemyMap[(HighPriority + i) % 8].end(), VecAdd);
			}
			HuntEnd = false;
			Expand = false;
		}
	}
	return Monsternum;
}

