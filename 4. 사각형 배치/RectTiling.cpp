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

//Default Unit Structure.
struct Grid {
	int value;
	Status status;
	Grid() : value(0), status(alive) {}
	Grid(int i) {}
	Grid(int av, Status as) : value(av), status(as) {}
	Grid(const Grid& obj) : Grid(obj.value, obj.status) {}
};

//Cycling Input Stream.
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

//To make usase of array easily, I scope map and make functions.
class RectMap {
private:	
	static const Grid  edgegrid;
	std::array< std::array<Grid, 11U>, 11U> mapdata;
public:
	RectMap() {
		std::fill(mapdata[0].begin(), mapdata[0].end(), RectMap::edgegrid);
		std::fill(mapdata[10].begin(), mapdata[10].end(), RectMap::edgegrid);
		for (int i = 1; i < 10; i++) {			
			mapdata[i][0] = mapdata[i][10] = RectMap::edgegrid;
			mapdata[i][1].status = mapdata[i][9].status = danger;
			mapdata[1][i].status = mapdata[9][i].status = danger;
		}
		mapdata[1][1].status = dead;
		mapdata[9][1].status = dead;
		mapdata[1][9].status = dead;
		mapdata[9][9].status = dead;
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
const Grid RectMap::edgegrid(INT_MAX, dead);


//Program main algorithm.
class TileManger {
private:
	RectMap map;
	IStream in;

	std::vector<int> rectinven;
	std::deque<Coord2> recthistory;
	static const int maxhistory = 16;
	static const int maxinven = 5;

	bool isValidPoint(const Coord2& topleft){
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

	//If somerect's status is changed, there are continual changes.
	//If somerect changes to danger, there might be somerect which changes to danger or dead.
	//It is a same case if somerect changes to dead.
	void CheckStatus(int ay, int ax) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; i++) {
				if (!CheckDanger(ay, ax, ay + i, ax + j)){
					//If StatusChange Occur or some surrounding rect's status is dead, map is additionally checked.
					//If status is changed to alive from danger, additional check is not needed for dead rect cannot be alive or danger.					
					//First two args is one of just tiled rect. So their status cannot be changed to danger or dead.
					//Howerver, them can be changed to alive form dead.
					//Therefore, we might just check last two args.
					CheckDead(ay + i, ax + j);					
				}
			}
		}
	}

	//Check Two Points if one of them is in danger or not.
	//true means change occurs
	bool CheckDanger(int fy, int fx, int sy, int sx) {		
		Grid* bigger, *smaller;
		if (map[fy][fx].value > map[sy][sx].value) {
			bigger = &map[fy][fx];
			smaller = &map[sy][sx];
		}
		else {
			bigger = &map[sy][sx];
			smaller = &map[fy][fx];
		}
		if (bigger->status == dead || smaller->status == dead) {
			return true;
		}
		if (bigger->value - smaller->value >= 10 && bigger->value >= 2 * smaller->value) {
			if (smaller->status == danger)
				return false;
			//if change is not occured, return false.
			smaller->status = danger;			
			return true;
		}
		else {
			//If it chang from danger to alive, surrounding rect is not affected.
			smaller->status = alive;	
			return false;
		}
	}

	bool CheckDead(int ay, int ax){
		//If Dead rect is found, then additional change can be occured
	}

	bool MakeDanger(int ay, int ax) {
		//if any rect is changed into dead, surrounding rects are also changed into danger,
		//and it is need to checkDead continually.

	}

	bool MakeDead() {
		3 = 2;
	}

	bool InDead(const int fy, const int fx){
		if (map[fy + 1][fx].status != alive&&
			map[fy - 1][fx].status != alive&&
			map[fy][fx + 1].status != alive&&
			map[fy][fx - 1].status != alive) {
			map[fy][fx].status = dead;
			return true;
		}
		else if (map[fy + 1][fx].status == dead && map[fy - 1][fx].status == dead) {
			map[fy][fx].status = dead;
			return true;
		}
		else if (map[fy][fx + 1].status == dead && map[fy][fx - 1].status == dead){
			map[fy][fx].status = dead;
			return true;
		}
		else
			return false;
	}
	
	bool InDead(const Coord2& obj){
		return InDead(obj.y, obj.x);
	}

	//Put Rectangle on map. obj means its topleft coord. rectval is value of rect
	//Status is changed ONLY if surrounding rects' value is changed
	void TileRect(Coord2& obj, int rectval) {
		map[obj.y][obj.x].value += rectval;
		map[obj.y + 1][obj.x].value += rectval;
		map[obj.y][obj.x + 1].value += rectval;
		map[obj.y + 1][obj.x + 1].value += rectval;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; i++) {
				//surronded rect's value is changed.
				//then, check changed rect's surrond
				CheckStatus(obj.y + i, obj.x + j);
			}
		}
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