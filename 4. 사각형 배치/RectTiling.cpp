#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <array>
#include <sstream>
#include <deque>

using std::endl;
using std::string;

#undef INT_MAX
#define INT_MAX 0x7fffffff
#define _LOCAL

#ifdef _LOCAL
#include<chrono>
#include<ctime>
std::ostream& out = std::cout;
std::chrono::time_point<std::chrono::system_clock> tstart, tend;
auto NowTime = std::chrono::system_clock::now;
#else
std::ofstream out("output.txt");
#endif

typedef unsigned int num;

struct Coord2 {
	int x, y;
	Coord2(int ax, int ay) : x(ax), y(ay){}
	Coord2(const Coord2& obj) :Coord2(obj.x, obj.y){}
	Coord2() :Coord2(0, 0) {}

	bool operator ==(const Coord2& obj) const{
		return (x == obj.x) && (y == obj.y);
	}
	bool operator !=(const Coord2& obj) const {
		return !(obj == *this);
	}
};

enum Status { alive = 0, danger = 1, dead = 2 };

struct Grid {
	int value;
	Status status;
	Grid() : value(0), status(alive) {}
	Grid(int av, Status as) : value(av), status(as) {}
	Grid(const Grid& obj) : Grid(obj.value, obj.status) {}
};

class IStream {
private:
	const std::array<int, 16U> data{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2 } };
	std::_Array_const_iterator<int, 16U> index;
	const std::_Array_const_iterator<int, 16U> beginindex = data.begin();
	const std::_Array_const_iterator<int, 16U> endindex = data.end();

	int GetNum() {
		if (index != data.end())
			return *(index++);
		else {
			index = beginindex;
			return *(index++);
		}
	}
public:
	IStream() {
		index = beginindex;
	}
	IStream& operator>>(int& value) {
		value = GetNum();
		return *this;
	};
};

class RectMap {
private:	
	
	const Grid  edgegrid = Grid(INT_MAX, danger);
	std::array< std::array<Grid, 11U>, 11U> mapdata;

	void CheckStatus(int ay, int ax) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; i++) {
				CheckDanger(ay, ax, ay + 1, ax + 1);
			}
		}
	}
	
	inline void CheckDanger(int fy, int fx, int sy, int sx) {
		Grid* bigger, *smaller;
		if (mapdata[fy + 1][fx + 1].value > mapdata[sy + 1][sx + 1].value) {
			bigger = &mapdata[fy + 1][fx + 1];
			smaller = &mapdata[sy + 1][sx + 1];
		}
		else {
			bigger = &mapdata[sy + 1][sx + 1];
			smaller = &mapdata[fy + 1][fx + 1];
		}
		if (bigger->status == dead || smaller->status == dead) {

		}
		if (bigger->value - smaller->value >= 10 && bigger->value >= 2 * smaller->value) {
			smaller->status = danger;
		}
		else {
			smaller->status = alive;
		}
	}
public:

	RectMap() {
		std::fill(mapdata[0].begin(), mapdata[0].end(), edgegrid);
		std::fill(mapdata[10].begin(), mapdata[10].end(), edgegrid);
		for (int i = 1; i < 10; i++) {			
			mapdata[i][0] = mapdata[i][10] = edgegrid;
		}		
	}

	void TileRect(Coord2& obj, int rectval) {
		mapdata[obj.y][obj.x].value += rectval;
		mapdata[obj.y + 1][obj.x].value += rectval;
		mapdata[obj.y][obj.x + 1].value += rectval;
		mapdata[obj.y + 1][obj.x + 1].value += rectval;
	}

#pragma region RectMapOperator
	std::array<Grid, 11U>& operator[](const int& value){
		return mapdata[value + 1];
	}
	Grid& operator[](const Coord2& value) {
		return mapdata[value.y + 1][value.x + 1];
	}
	friend std::ostream& operator << (std::ostream& ostm, const RectMap& obj){
		for (int y = 1; y < 10; y++) {
			for (int x = 1; x < 10; x++) {
				ostm << obj.mapdata[y][x].value<< ' ';
			}
			ostm << endl;
		}
		return ostm;
	}
#pragma endregion
};

class TileManger {
private:
	RectMap map;
	IStream in;

	std::vector<int> rectinven;
	std::deque<Coord2> recthistory;
	static const int maxhistory = 16;
	static const int maxinven = 5;

	inline bool isValidPoint(const Coord2& topleft){
		bool alreadydid = false;
		for (auto i = recthistory.begin(); i < recthistory.end(); i++) {
			if (*i == topleft) {
				alreadydid = true;
				break;
			}
		}
		return(map[topleft.y - 1][topleft.x].status == dead &&
			map[topleft.y - 1][topleft.x + 1].status == dead &&
			map[topleft.y][topleft.x + 2].status == dead &&
			map[topleft.y + 1][topleft.x + 2].status == dead &&
			map[topleft.y + 2][topleft.x + 1].status == dead &&
			map[topleft.y + 2][topleft.x].status == dead &&
			map[topleft.y + 1][topleft.x - 1].status == dead &&
			map[topleft.y][topleft.x - 1].status == dead)
			|| alreadydid;
	}

	void FindPropCoord() {

	}

public:
	TileManger() {
		rectinven.reserve(5);
		for (int i = 0; i < maxinven; i++){
			int tmp;
			in >> tmp;
			rectinven.push_back(tmp);
		}
	}
	~TileManger() {
		out << map;
	}

};



int main() {
#ifdef _LOCAL
	tstart = NowTime();
#endif

	TileManger data;
#ifdef _LOCAL
	tend = NowTime();
	out << "Total Operating time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	return 0;
}