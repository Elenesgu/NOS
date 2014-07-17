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
fstream out("Output.txt");
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

typedef vector<Unit> Map;
typedef pair<Unit*, double> UnitWeight;

Map EnemyMap;
array<num, 8> DirPriority;
size_t HighPriority;
Unit User;
string UserLog;
num doTime;
num Monsternum = 0;

num pickCount;

bool HuntEnd = false;

inline static double getLength(const Vector2& origin, const Vector2& target) {
	return sqrt(pow(static_cast<double>(origin.x) - static_cast<double> (target.x), 2)
		+ pow(static_cast<double>(origin.y) - static_cast<double>(target.y), 2));
}

inline static double getWeight(const Unit& origin, const Unit& target, double Length) {
	return static_cast<double> (target.exp) / (Length + static_cast<double> (target.time));
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

static vector<UnitWeight>  FindTarget();
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

	pickCount = uCase / 100;
	if (pickCount == 0) {
		pickCount = 1;
	}

	counter = 0;
	while (counter < uCase) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap.push_back(Unit(tempX, tempY, tempTime, tempExp));
		counter++;
	}
	counter = 0;

	num Max = 0;

	auto result = doTest();
	out << doTime - User.time << endl << User.exp << endl;

	out << result << endl << UserLog;

	return 0;
}

static void Hunt(Unit& enemy) {
	if (HuntEnd) {
		return;
	}
	PrintLog(enemy.coord);
	User.exp += enemy.exp;
	enemy.exp = 0;
	User.time -= enemy.time;
	User.time -= static_cast<int> (getLength(User.coord, enemy.coord));
	User.coord = enemy.coord;
	Monsternum++;
}

static vector<UnitWeight> FindTarget() {
	UnitWeight tmp;

	vector<UnitWeight> weightEnemy;
	for (auto itr = EnemyMap.begin(); itr != EnemyMap.end(); itr++) {
		tmp.first = &(*itr);
		tmp.second = getWeight(User, (*itr));
	}
	sort(weightEnemy.begin(), weightEnemy.end(), [](UnitWeight a, UnitWeight b) {return a.second < b.second; });
	auto itr = EnemyMap.begin() + 1;
	return vector<UnitWeight>(itr, itr + pickCount);
}

static num doTest() {
	bool Expand = true;
	while (User.time > 0 && !HuntEnd) {

	}
	return Monsternum;
}

