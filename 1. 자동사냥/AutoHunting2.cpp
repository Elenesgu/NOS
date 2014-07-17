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
#include<ctime>
fstream in;
ostream& out = cout;
#else
fstream in("Input.txt");
fstream out("Output.txt");
#endif

typedef unsigned int num;

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
	bool valid;
	size_t index;
	Unit(num ax, num ay, num atime, num aexp, bool avalid, size_t aindex) : coord(ax, ay), time(atime), exp(aexp), valid(avalid), index(aindex) {};
	Unit(num ax, num ay, num atime) : Unit(ax, ay, atime, 0, true, 0) {};
	Unit() : Unit(0, 0, 0, 0, true, 0) {};
	Unit(const Unit& obj) : Unit(obj.coord.x, obj.coord.y, obj.time, obj.exp, obj.valid, obj.index) {};
};

typedef vector<Unit> Map;
typedef pair<Unit*, double> UnitWeight;

struct Result {
	num time;
	num exp;
	vector<Vector2> Hunted;
	Result() :time(0),exp(0),Hunted(){};
	Result(const Result& obj) : time(obj.time), exp(obj.exp), Hunted(obj.Hunted){};
};

array< vector<UnitWeight>, 5001> DynamicMemory;
Map EnemyMap;
array<num, 8> DirPriority;
size_t HighPriority;
Unit User;
num doTime;
num Monsternum = 0;

num pickCount(4);

bool HuntEnd = false;

inline static double getLength(const Vector2& origin, const Vector2& target) {
	return sqrt(pow(static_cast<double>(origin.x) - static_cast<double> (target.x), 2)
		+ pow(static_cast<double>(origin.y) - static_cast<double>(target.y), 2));
}

inline static double getWeight(const Vector2& origin, const Unit& target, double Length) {
	return static_cast<double> (target.exp) / (Length + static_cast<double> (target.time));
	// Reward / Time (to get the reward)
}

inline static double getWeight(const Vector2& origin, const Unit& target) {
	return getWeight(origin, target, (getLength(origin, target.coord)));
	// Reward / Time (to get the reward)
}

static vector<UnitWeight>  FindTarget(Unit origin);
static Result doTest();
static Result doTest(Unit Clone, Result Base);

int main() {
	num tempX, tempY, tempTime, tempExp;
	size_t uCase, counter;
#ifdef _LOCAL
	time_t a = time(NULL);
	int inputnum;
	out << "Case Num: ";
	cin >> inputnum;
	in.open("input" + to_string(inputnum) + ".txt");
	if (!in) {
		out << "No File" << endl;
		return 0;
	}
#endif

	in >> tempX >> tempY >> doTime;

	User = Unit(tempX, tempY, doTime);

	in >> uCase;

	if (pickCount > uCase) {
		pickCount = uCase - 1;
	}
	pickCount = 20000 / uCase;
#ifdef _LOCAL
	out << "Branches: " << pickCount << endl;
#endif

	counter = 0;
	while (counter < uCase) {
		in >> tempX >> tempY >> tempTime >> tempExp;
		EnemyMap.push_back(Unit(tempX, tempY, tempTime, tempExp, true, counter + 1));
		counter++;
	}
	counter = 0;

	num Max = 0;

	auto result = doTest();
#ifdef _LOCAL
	out << "Time: " << result.time << endl << "EXP: " << result.exp << endl << result.Hunted.size() << endl;
	for (auto itr = result.Hunted.begin(); itr != result.Hunted.end(); itr++) {
		out << (*itr).x << ' ' << (*itr).y << endl;
	}
#else
	out << result.time << endl << result.exp << endl << result.Hunted.size() << endl;
	for (auto itr = result.Hunted.begin(); itr != result.Hunted.end(); itr++) {
		out << (*itr).x << ' ' << (*itr).y << endl;
	}
#endif

#ifdef _LOCAL
	out << "Cost: " << time(NULL) - a << endl;
	in.close();
#endif

	return 0;	
}

static vector<UnitWeight> FindTarget(Unit origin) {
	vector<UnitWeight> weightEnemy;
	if (DynamicMemory[origin.index].empty()) {
		for (auto itr = EnemyMap.begin(); itr != EnemyMap.end(); itr++) {
			if ((*itr).valid) {
				weightEnemy.push_back(UnitWeight(&(*itr), getWeight(origin.coord, (*itr))));
			}
		}
		sort(weightEnemy.begin(), weightEnemy.end(), [](UnitWeight a, UnitWeight b) {return a.second > b.second; });
		DynamicMemory[origin.index] = weightEnemy;
	}
	else {
		weightEnemy = DynamicMemory[origin.index];
	}
	vector<UnitWeight> ResultVec;
	size_t loopcounter = (pickCount > weightEnemy.size()) ? weightEnemy.size() : pickCount;
	for (size_t i = 0; i < loopcounter; i++) {
		ResultVec.push_back(weightEnemy[i]);
	}
	
	return vector<UnitWeight>(ResultVec);
}

static Result doTest() {	
	return Result (doTest(User, Result()) );
}

static Result doTest(Unit Clone, Result Base) {
	vector<Result> Results;
	vector<UnitWeight> Targets ( FindTarget(Clone) );	
	for (size_t i = 0; i < Targets.size(); i++){
		if ((Targets[i].first->time + getLength(Clone.coord, Targets[i].first->coord)) + Base.time > doTime) {			
			break;
		}
		else {
			Result tmpresult(Base);
			tmpresult.time += static_cast<num> ((Targets[i].first->time + getLength(Clone.coord, Targets[i].first->coord)));
			tmpresult.exp += Targets[i].first->exp;
			tmpresult.Hunted.push_back(Targets[i].first->coord);
			Targets[i].first->valid = false;
			if (pickCount > 1) {
				pickCount--;
				Results.push_back(doTest(*(Targets[i].first), tmpresult));
				pickCount++;
			}
			else {
				Results.push_back(doTest(*(Targets[i].first), tmpresult));
			}
			Results.push_back(tmpresult);
			Targets[i].first->valid = true;
		}
	}

	Result* maxResult;
	num maxEXP = 0;
	if (Results.empty()) {
		return Result(Base);
	}
	else {
		for (auto itr = Results.begin(); itr != Results.end(); itr++){
			if (maxEXP < (*itr).exp) {
				maxResult = &(*itr);
				maxEXP = maxResult->exp;
			}
			else if (maxEXP == (*itr).exp) {
				if (maxResult->time > (*itr).time) {
					maxResult = &(*itr);
				}
			}
		}
	}
	return Result(*maxResult);
}