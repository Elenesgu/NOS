#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <array>
#include <sstream>
#include <deque>
#include <list>

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

class ScopedMap : public std::array<Grid, 11U> {
public:	
	Grid& operator[](const int& v) const {
		return const_cast<Grid&>(this->at(v + 1));
	}
};

//To make usase of array easily, I scope map and make functions.
class RectMap {
private:	
	
	static const Grid  edgegrid;
	std::array<ScopedMap, 11U> mapdata;
public:
	RectMap() {
		std::fill(mapdata[0].begin(), mapdata[0].end(), RectMap::edgegrid);
		std::fill(mapdata[10].begin(), mapdata[10].end(), RectMap::edgegrid);
		for (int i = 1; i < 10; i++) {			
			mapdata[i][-1] = mapdata[i][9] = RectMap::edgegrid;
			mapdata[i][0].status = mapdata[i][8].status = danger;
			mapdata[1][i - 1].status = mapdata[9][i - 1].status = danger;
		}
		mapdata[1][0].status = dead;
		mapdata[9][0].status = dead;
		mapdata[1][8].status = dead;
		mapdata[9][8].status = dead;
	}
#pragma region RectMapOperator
	ScopedMap& operator[](const int& v){
		return mapdata[v + 1];
	}
	Grid& operator[](const Coord2& v) {
		return mapdata[v.y + 1][v.x];
	}
	friend std::ostream& operator << (std::ostream& ostm, const RectMap& obj){
		for (int y = 1; y < 10; y++) {
			for (int x = 0; x < 9; x++) {
				ostm << obj.mapdata[y][x].value << ", " << obj.mapdata[y][x].status <<'\t';
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
	static const int maxhistory = 16;
	static const int maxinven = 5;

	int rectinven[maxinven];	
	Coord2 recthistory[maxhistory];
	std::vector<Grid*> nextdanger;
	std::vector<Grid*> nextlive;
	std::vector<Coord2> nextcheck;

	void MainLogic() {
#ifdef _LOCAL
		out << endl << map;
#endif
		nextdanger.clear();
		nextlive.clear();
		nextcheck.clear();
		Coord2 TopLeft;
		int TileNum;
#ifdef _LOCAL
		int a, b, n, m;
		out << "x, y: ";
		std::cin >> a >> b;
		out << "InvenNum" << endl;
		for (int i = 0; i < maxinven; i++) {
			out << rectinven[i] << ' ';
		}
		out << ": ";		
		std::cin >> n;
		TileNum = rectinven[n];
		in >> m;
		rectinven[n] = m;
		TopLeft = Coord2(a, b);
#else
		TopLeft = FindPropCoord(TileNum);
#endif
		TileRect(TopLeft, TileNum);
	}

#pragma region GamePlay
	bool isValidPoint(const Coord2& topleft){
		bool alreadydid = false;
		return(alreadydid || map[topleft.y - 1][topleft.x].status == dead &&
			map[topleft.y - 1][topleft.x + 1].status == dead &&
			map[topleft.y][topleft.x + 2].status == dead &&
			map[topleft.y + 1][topleft.x + 2].status == dead &&
			map[topleft.y + 2][topleft.x + 1].status == dead &&
			map[topleft.y + 2][topleft.x].status == dead &&
			map[topleft.y + 1][topleft.x - 1].status == dead &&
			map[topleft.y][topleft.x - 1].status == dead);
	}

	//Put Rectangle on map. obj means its topleft coord. rectval is value of rect
	//Status is changed ONLY if surrounding rects' value is changed
	void TileRect(Coord2& obj, int rectval) {
		map[obj.y][obj.x].value += rectval;
		map[obj.y + 1][obj.x].value += rectval;
		map[obj.y][obj.x + 1].value += rectval;
		map[obj.y + 1][obj.x + 1].value += rectval;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				//surronded rect's value is changed.
				//then, check changed rect's surrond
				CheckStatus(obj.y + i, obj.x + j);
			}
		}


#ifdef _LOCAL
		out << endl <<map;
#endif
		for (Grid* obj : nextlive)
			obj->status = alive;
		for (Grid* obj : nextdanger)
			obj->status = danger;
		for (Coord2 obj : nextcheck)
			CheckDead(obj);
	}

	Coord2 FindPropCoord(int& N) {
		Coord2 PropCoord;
		int minPrior = INT_MAX;

		for (PropCoord.y = 0; PropCoord.y < 9; PropCoord.y++) {
			for (PropCoord.x = 0; PropCoord.x < 9; PropCoord.x++) {
				
			}
		}
		return Coord2(PropCoord);
	}
#pragma endregion

#pragma region GameRule
	/*
	한글 주석 설명:
	맵(9x9의 사각형으로 이루어져있다.)에서 상태가 변하기 위해선, 다음과 같은 조건이 필요하다.
	dead -> danger = 즉, 죽은 셀 옆이 위험으로 변한다.
	any -> dead = 죽은 셀 사이에 있으면 죽는다. (상하 또든 좌우)
	any -> dead = 죽거나 위험한 셀 사이에 있으면 죽는다 (상하좌우 모두)
	any -> dead = 잘못된 사각형을 두면 칸들이 죽는다.
	alive -> danger = 숫자 차이가 클경우 위험상태가 된다.
	danger -> alive = 차이가 원래대로 돌아올 경우 원상복귀 된다.

	A.
	사용자가 사각형을 놓을 때 연쇄적으로 상태가 변화한다.
	즉, 먼저 사용자가 사각형을 놓는다. (올바른 위치라고 가정한다. 즉 죽는 사각형으로 놓지 않는다.
	그러면 1. 위험이 생기거나, 2. 위험이 사라지거나, 3. 아무 상태변화도 나타나지 않게 된다.
	1. 위험이 생긴다 -> 위험은 다른 트리거의 조건일 수 있으므로, 연쇄적으로 반응을 일으킨다.
	2. 위험이 사라진다 -> 위험 상태에 영향을 받으면 무조건 '죽음'이 된다. 그러나 죽음은 위험으로 바뀌지 않으므로, 어떤 셀의 위험이 사라진다 하더라도 아무 변화가 없다.
	3. 아무 변화가 없다 -> 처음으로 돌아가 다시 사각형을 놓는다.

	B.
	1의 경우 추가 반응이 필요하다.
	위험 상태로 어떤 셀이 변했으므로, 변한 셀의 상하좌우를 조사하여, 죽었는지 확인한다.
	그러면 1. 죽는 셀이 존재하거나, 2. 위험상태가 될 뿐 죽는 셀이 존재하지 않는다.
	1. 죽는 셀이 존재한다.	-> 둘러 싸인 셀이 죽게 된다. 이럴 경우 주위 셀은 이미 위험이므로 위험을 더이상 체크하지 않는다.
						-> 하지만 끼인 셀으로 죽는 셀이 있는지 확인한다. 없을경우 끝낸다.
						-> 죽는 셀 사이에 낀 셀이 죽게 된다. 이런 경우 위험셀이 생성된다. 그러면 이 루틴의 처음으로 간다.( 재귀호출)
	2. 죽는 셀이 존재하지 않는다. -> 더이상 변화가 없으므로 처음으로 돌아가 다시 사각형을 놓는다.

first: call A
	A. CheckStatus
	if danger cell occur(calculated by Checkdanger)
	-> call B
	else 
	-> to first

	B. CheckDead
	if changed cell occur(calculated by check dead)	
	-> call checkdead again

	-> call B.
	else 
	-> to first

	*/

	//If somerect's status is changed, there are continual changes.
	//If somerect changes to danger, there might be somerect which changes to danger or dead.
	//It is a same case if somerect changes to dead.
	void CheckStatus(const int ay, const int ax) {
		for (int i = -1; i < 2; i += 2) {
			if (CheckDanger(ay, ax, ay + i, ax)){
				//If StatusChange Occur or some surrounding rect's status is dead, map is additionally checked.
				//If status is changed to alive from danger, additional check is not needed for dead rect cannot be alive or danger.					
				//First two args is one of just tiled rect. So their status cannot be changed to danger or dead.
				//Howerver, them can be changed to alive form dead.
				//Therefore, we might just check last two args.
				/*---------------------------------------------*/
				//Only Tiled grid's status change to danger or dead
				//Mission condition changerd.
				nextcheck.push_back(Coord2(ax, ay));
			}
			if (CheckDanger(ay, ax, ay, ax + i)){
				nextcheck.push_back(Coord2(ax, ay));
			}
		}
	}

	//Check Two Points if one of them is in danger or not.
	//true means change occurs
	bool CheckDanger(const int fy, const int fx, const int sy, const int sx) {
		Grid* bigger, *smaller;
		int bx, by;
		if (map[fy][fx].value > map[sy][sx].value) {
			bigger = &map[fy][fx];
			bx = fx;
			by = fy;
			smaller = &map[sy][sx];
		}
		else {
			bigger = &map[sy][sx];
			bx = sx;
			by = sy;
			smaller = &map[fy][fx];
		}

		if (bigger->status == dead || smaller->status == dead) {
			MakeDead(fy, fx);
			MakeDead(sy, sx);
			return true;
		}
		if (bigger->value - smaller->value >= 10 && bigger->value >= 2 * smaller->value) {
			if (bigger->status == danger)
				return false;
			//if change is not occured, return false.
			nextdanger.push_back(bigger);
			return true;
		}
		else {
			//If it chang from danger to alive, surrounding rect is not affected.
			if (bigger->status == danger) {
				if (!surroundDead(by, bx)) {
					nextlive.push_back(bigger);
				}
			}
			return false;
		}
	}

	void CheckDead(const int ay, const int ax){
		//If Dead rect is found, then additional change can be occured
		if (map[ay][ax].value == INT_MAX)
			return;
		for (int i = -1; i < 2; i+=2) {
			InDead(ay + i, ax);
			InDead(ay, ax + i);
		}
	}

	void CheckDead(const Coord2 obj) {
		CheckDead(obj.y, obj.x);
	}

	void MakeDead(const int ay, const int ax) {
		if (map[ay][ax].value == INT_MAX)
			return;
		map[ay][ax].status = dead;
		for (int i = -1; i < 2; i+=2) {
			if (map[ay + i][ax].status == alive) {
				map[ay + i][ax].status = danger;
				CheckDead(ay + i, ax);
			}
			if (map[ay][ax + i].status == alive) {
				map[ay][ax + i].status = danger;
				CheckDead(ay, ax + i);
			}
		}
	}

	//      danger
	//danger  &   danger
	//      danger
	void InDead(const int fy, const int fx){
		if (map[fy][fx].value == INT_MAX)
			return;
		if (map[fy + 1][fx].status != alive&&
			map[fy - 1][fx].status != alive&&
			map[fy][fx + 1].status != alive&&
			map[fy][fx - 1].status != alive) {
			map[fy][fx].status = dead;
			//dead  & dead 
			//
			//  dead
			//   &
			//  dead
			if (fy != 8 && map[fy + 2][fx].status == dead)
				MakeDead(fy + 1, fx);
			if (fy != 0 && map[fy - 2][fx].status == dead)
				MakeDead(fy - 1, fx);
			if (fx != 8 && map[fy][fx + 2].status == dead)
				MakeDead(fy, fx + 1);
			if (fx != 0 && map[fy][fx - 2].status == dead)
				MakeDead(fy, fx - 1);
		}
	}

	bool surroundDead(const int ay, const int ax) {
		for (int i = -1; i < 2; i += 2) {
			if (map[ay + i][ax].status == dead) {
				return true;
			}
			if (map[ay][ax + i].status == dead) {
				return true;
			}
		}
		return false;
	}
#pragma endregion

public:
	TileManger() {
		for (int i = 0; i < maxinven; i++){
			int tmp;
			in >> tmp;
			rectinven[i] = tmp;
		}
		while (1) {
			MainLogic();
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