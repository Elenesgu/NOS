#include<iostream>

using std::endl;

#ifdef _LOCAL
#include<chrono>
#include<ctime>
std::ostream& out = std::cout;
std::chrono::time_point<std::chrono::system_clock> tstart, tend;
auto NowTime = std::chrono::system_clock::now;
#else
#include<fstream>
std::ofstream out("output.txt");
#endif

int main() {

}