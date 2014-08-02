#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <array>
#include <sstream>

using std::endl;
using std::string;

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

static IStream in;

int main() {
#ifdef _LOCAL
	tstart = NowTime();
#else	
#endif


#ifdef _LOCAL
	tend = NowTime();
	out << "Total Operating time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	return 0;
}