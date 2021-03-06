#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ios>
#include <queue>
#include <cstdlib>

using std::string;
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

typedef unsigned int num;

#pragma pack(push, 1)

struct BitmapFileHeader {
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
};

struct BitmapInfoHeader {
	unsigned int    biSize;
	int             biWidth;
	int             biHeight;
	unsigned short  biPlanes;
	unsigned short  biBitCount;
	unsigned int    biCompression;
	unsigned int    biSizeImage;
	int             biXPelsPerMeter;
	int             biYPelsPerMeter;
	unsigned int    biClrUsed;
	unsigned int    biClrImportant;
};

struct BitColor {
	unsigned char B, G, R;
	BitColor(unsigned char aG, unsigned char aB, unsigned char aR) :G(aG), B(aB), R(aR) {}
	BitColor() : BitColor(0, 0, 0) {}
	BitColor(const BitColor& obj) : BitColor(obj.G, obj.B, obj.R) { }

	bool operator ==(const BitColor& obj) const{
		return (R == obj.R) && (G == obj.G) && (B == obj.B);
	}
	bool operator !=(const BitColor& obj) const {
		return !(obj == *this);
	}
};

#pragma pack(pop)


struct Coord2 {
	int x, y;
	Coord2(int ax, int ay) : x(ax), y(ay){}
	Coord2(const Coord2& obj) :Coord2(obj.x, obj.y){}
	Coord2():Coord2(0, 0) {}

	bool operator ==(const Coord2& obj) const{
		return (x == obj.x) && (y == obj.y);
	}
	bool operator !=(const Coord2& obj) const {
		return !(obj == *this);
	}
};

struct Node {
	Coord2 TopLeft;
	num Width, Height;
	BitColor Color;
	num order;
	Node(int x, int y, num w, num h, BitColor bc) :TopLeft(x, y), Width(w), Height(h), Color(bc) {}
	Node(Coord2 cord, num w, num h, BitColor bc) :TopLeft(cord), Width(w), Height(h), Color(bc) {}
	Node() :TopLeft(-1, -1) {} //Null reference
	friend std::ostream& operator<<(std::ostream& ost, const Node& obj) {
		ost << obj.Width << ' ' << obj.Height << ' ' << static_cast<int> (obj.Color.R) << ' ' << static_cast<int> (obj.Color.G) << ' ' << static_cast<int> (obj.Color.B) << endl;
		return ost;
	}
};

struct Edge{
	int First, Second;

	friend std::ostream& operator<<(std::ostream& ost, const Edge& obj) {
		ost << obj.First << ' ' << obj.Second << endl;
		return ost;
	}
};

struct tmpEdge {
	std::vector<Coord2> Earth;
	tmpEdge() : Earth(1, Coord2(-1, -1)) {}
	tmpEdge(const std::vector<Coord2> obj) : Earth(obj.begin(), obj.end()) {}
};

class Image {
private:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	FILE* fstream;
	std::ofstream output;
	int width, height;

	//std::vector< std::vector<BitColor> > Bitmap;
	BitColor** Bitmap;
	std::vector< std::vector<bool> > Visited;

	static BitColor WhiteBit;
	static BitColor BlackBit;
	std::vector<Node> Nodes;
	std::vector<Edge> Edges;
	std::vector<tmpEdge> tmpEdges;
public:
	Image() : fstream() {}
	Image(string filename, string mod) {
		fopen_s(&fstream, filename.c_str(), mod.c_str());
		ReadFile();
		Search();
		MakeEdge();
		PrintResult();
#ifdef _W_FILE
		output.open("output.txt", std::ios::out);
		WriteFile("output.bmp");
#endif

	}
	Image(string filename) : Image(filename, "rb") {}
	void ReadFile();
	void Search();
	void MakeEdge();
	void PrintResult();
	Node FindNode(Coord2 tl);
	tmpEdge FindEdge(Coord2 tl);
#ifdef _W_FILE
	void WriteFile(string filename);
#endif
	~Image() {
		fclose(fstream);
		for (int i = 0; i < height; i++){
			free(Bitmap[i]);
		}
		free(Bitmap);
#ifdef _LOCAL
		int a;
		std::cin >> a;
#endif
#ifdef _W_FILE
		output.close();
#endif
	}
};

BitColor Image::WhiteBit = BitColor(255, 255, 255);
BitColor Image::BlackBit = BitColor(0, 0, 0);

int main() {
	string IfileName;

#ifdef _LOCAL
	
	out << "Input File Name(not include .bmp): ";
	std::cin >> IfileName;
	IfileName += ".bmp";
	tstart = NowTime();
#else
	IfileName = "input.bmp";
#endif

	Image Data(IfileName);

#ifdef _LOCAL
	tend = NowTime();
	out << "Total Operating time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	return 0;
}

void Image::ReadFile() {
	if (!fstream){
		out << "File not found" << endl;
		exit(-1);
	}
	fread(reinterpret_cast<char*>(&fileHeader), sizeof fileHeader, 1, fstream);
	if (fileHeader.bfType != ('B' | (static_cast<int>('M') << 8))) {
		out << "File is not a bitmap" << endl;
		return;
	}

	fread(reinterpret_cast<char*>(&infoHeader), sizeof infoHeader, 1, fstream);

	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	const int bitsPerPixel = infoHeader.biBitCount;
	const int bytesPerPixel = bitsPerPixel / 8;
	const int pitch = (width * bytesPerPixel + 3) & ~3;
	const int dataSize = pitch * height;

	if (width > 8192 || width <= 0 ||
		height > 8192 || height <= 0 ||
		infoHeader.biPlanes != 1 ||
		bitsPerPixel != 24 ||
		infoHeader.biCompression != 0 ||
		static_cast<int>(infoHeader.biSizeImage) != dataSize
		) {
		out << "Unsupported Format" << endl;
		out << "Width: " << width << ", Height: " << height << endl;
		return;
	}
	const int dataBeginPos = static_cast<int> (ftell(fstream));
	if (dataBeginPos != static_cast<int>(fileHeader.bfOffBits))
	{
		out << "Unsupported Format" << endl;
		return;
	}
	BitColor tmp;
	Bitmap = (BitColor**) malloc(sizeof(BitColor*) * height);
	Visited = std::vector<std::vector<bool> >(height, std::vector<bool>(width, false));
	int trash;
	const int padding = width % 4;

#ifdef _LOCAL
	tend = NowTime();
	out << "Allocation End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	for (int y = height - 1; y >= 0; y--) {
		Bitmap[y] = (BitColor*)malloc(sizeof(BitColor)* width);
		fread(reinterpret_cast<char *>(Bitmap[y]), sizeof(BitColor)* width, 1, fstream);
		fread(reinterpret_cast<char *>(&trash), sizeof(char) * padding, 1, fstream);

	}
#ifdef _LOCAL
	tend = NowTime();
	out << "File Read End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
}

void Image::Search() {
	int Nodeindex = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (Visited[y][x] == false) {
				if (Bitmap[y][x] == WhiteBit) {
					Visited[y][x] = true;
				}
				else if(!Visited[y][x]) {
					if (Bitmap[y][x] == BlackBit){
						//Edge
						tmpEdge tedge = FindEdge(Coord2(x, y));
						if (tedge.Earth[0] != Coord2(-1, -1)) {
							tmpEdges.push_back(tedge);
						}
					}
					else {
						//Node
						Node tmpnode = FindNode(Coord2(x, y));
						tmpnode.order = Nodeindex++;
						if (tmpnode.TopLeft != Coord2(-1, -1)){
							Nodes.push_back(tmpnode);
						}
					}
				}
			}
		}
	}
}

void Image::MakeEdge() {
#ifdef _LOCAL
	tend = NowTime();
	out << "Search End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	tmpEdge origin; 
	Edge target;
	Coord2 BaseCoord;
	for (size_t i = 0; i < tmpEdges.size(); i++)  {
		origin = tmpEdges[i];
		BaseCoord = origin.Earth[0];
		for (size_t index = 0; index < Nodes.size(); index++) {
			if ((Nodes[index].TopLeft.x <= BaseCoord.x) && (Nodes[index].TopLeft.x + static_cast<int>(Nodes[index].Width) >= BaseCoord.x)
				&& (Nodes[index].TopLeft.y <= BaseCoord.y) && (Nodes[index].TopLeft.y + static_cast<int>(Nodes[index].Height) >= BaseCoord.y)) {

				target.First = Nodes[index].order;
				break;
			}
		}
		for (size_t j = 1; j < origin.Earth.size(); j++) {			
			BaseCoord = origin.Earth[j];
			int tmporder;
			for (size_t index = 0; index < Nodes.size(); index++) {
				if ((Nodes[index].TopLeft.x <= BaseCoord.x) && (Nodes[index].TopLeft.x + static_cast<int>(Nodes[index].Width) >= BaseCoord.x)
					&& (Nodes[index].TopLeft.y <= BaseCoord.y) && (Nodes[index].TopLeft.y + static_cast<int>(Nodes[index].Height) >= BaseCoord.y)) {

					tmporder = Nodes[index].order;
					break;
				}
			}
			if (tmporder != target.First) {
				target.Second = tmporder;
				break;
			}
		}
		if (target.First > target.Second) {
			std::swap(target.First, target.Second);
		}
		Edges.push_back(target);
		
	}
}

void Image::PrintResult() {
#ifdef _LOCAL
	out << "Resut: " << endl;
#endif
	out << Nodes.size() << ' ' << Edges.size() << endl;
	for (size_t i = 0; i < Nodes.size(); i++){
		out << Nodes[i];
	}
	std::sort(Edges.begin(), Edges.end(), [](const Edge a, const Edge b) {
		return (a.First * 255 + a.Second) < (b.First * 255 + b.Second);
	});
	for (size_t i = 0; i < Edges.size(); i++){
		out << Edges[i];
	}
}

Node Image::FindNode(Coord2 tl) {
#ifdef _LOCAL
	static int countnum = 1;
	tend = NowTime();
	out << countnum++ << ")Node Found: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << " at " << tl.x << ", " << tl.y << endl;
#endif
	BitColor nColor = Bitmap[tl.y][tl.x];
	int initx = tl.x, inity = tl.y;
	for (; initx < width; initx++) {		
		if (Bitmap[inity][initx] != nColor) {
			break;
		}
	}
	for (; inity < height; inity++){
		if (Bitmap[inity][initx-1] != nColor){
			break;
		}
	}
	int nwidth, nheight;
	nwidth = initx - tl.x;
	nheight = inity - tl.y;
	if (nwidth == 1 || nheight == 1){
		Visited[inity][initx] = true;
		return Node();
	}
	else{
		for (int i = 0; i < nheight; i++){
			for (int j = 0; j < nwidth; j++){
				Visited[tl.y + i][tl.x + j] = true;
			}
		}
		return Node(tl, nwidth, nheight, nColor);
	}
}

tmpEdge Image::FindEdge(Coord2 tl) {
#ifdef _LOCAL
	static int countnum = 1;
	tend = NowTime();
	out << countnum++ << ")tmpEdge Found: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << " at " << tl.x << ", " << tl.y << endl;
#endif
	std::queue<Coord2> BFSqueue;
	Visited[tl.y][tl.x] = true;
	BFSqueue.push(tl);
	std::vector<Coord2> earth;
	Coord2 Curcoord;
	while (!BFSqueue.empty()){
		
		Curcoord = BFSqueue.front();
		BFSqueue.pop();
		if (Bitmap[Curcoord.y][Curcoord.x + 1] != WhiteBit) {
			if (Bitmap[Curcoord.y][Curcoord.x + 1] == BlackBit) {
				if (!Visited[Curcoord.y][Curcoord.x + 1]) {
					Visited[Curcoord.y][Curcoord.x + 1] = true;
					BFSqueue.push(Coord2(Curcoord.x + 1, Curcoord.y));
				}
			}
			else {
				earth.push_back(Coord2(Curcoord.x + 1, Curcoord.y));
			}

		}
		else {
			Visited[Curcoord.y][Curcoord.x + 1] = true;
		}
		if (Bitmap[Curcoord.y][Curcoord.x - 1] != WhiteBit) {
			if (Bitmap[Curcoord.y][Curcoord.x - 1] == BlackBit) {
				if (!Visited[Curcoord.y][Curcoord.x - 1]){
					Visited[Curcoord.y][Curcoord.x - 1] = true;
					BFSqueue.push(Coord2(Curcoord.x - 1, Curcoord.y));
				}
			}
			else {
				earth.push_back(Coord2(Curcoord.x - 1, Curcoord.y));
			}

		}
		else {
			Visited[Curcoord.y][Curcoord.x - 1] = true;
		}
		if (Bitmap[Curcoord.y + 1][Curcoord.x] != WhiteBit) {
			if (Bitmap[Curcoord.y + 1][Curcoord.x] == BlackBit) {
				if (!Visited[Curcoord.y + 1][Curcoord.x]){
					Visited[Curcoord.y + 1][Curcoord.x] = true;
					BFSqueue.push(Coord2(Curcoord.x, Curcoord.y + 1));
				}
			}
			else {
				earth.push_back(Coord2(Curcoord.x, Curcoord.y + 1));
			}

		}
		else {
			Visited[Curcoord.y + 1][Curcoord.x] = true;
		}
		if (Bitmap[Curcoord.y - 1][Curcoord.x] != WhiteBit) {
			if (Bitmap[Curcoord.y - 1][Curcoord.x] == BlackBit) {
				if (!Visited[Curcoord.y - 1][Curcoord.x]) {
					Visited[Curcoord.y - 1][Curcoord.x] = true;
					BFSqueue.push(Coord2(Curcoord.x, Curcoord.y - 1));
				}
			}
			else {
				earth.push_back(Coord2(Curcoord.x, Curcoord.y - 1));	
			}
		}
		else {
			Visited[Curcoord.y - 1][Curcoord.x] = true;
		}
	}
	if (earth.size() < 2){
		return tmpEdge();
	}
	Coord2 cFirst = earth.front();
	Coord2 cSecond;
	double maxDistance = 0;
	double tmpDistance;
	for (size_t i = 1; i < earth.size(); i++) {
		tmpDistance = pow(static_cast<double> (std::abs(cFirst.x - cSecond.x)), 2) + pow(static_cast<double>(abs(cFirst.y - cSecond.y)), 2);
		if (maxDistance < tmpDistance) {
			maxDistance = tmpDistance;
			cSecond = earth[i];
		}
	}
	
	return tmpEdge(earth);
}

//Not tested
#ifdef _W_FILE
void Image::WriteFile(string filename) {
	std::ofstream output(filename, std::ios::out);

	output.write(reinterpret_cast<char*>(&fileHeader), sizeof fileHeader);
	output.write(reinterpret_cast<char*>(&infoHeader), sizeof infoHeader);

	BitColor tmp;
	for (int y = height - 1; y >= 0; y--) {
		output.write(reinterpret_cast<char*> (&Bitmap[y]), sizeof(BitColor) * width);
	}
#ifdef _LOCAL
	tend = NowTime();
	out << "OutputFile Created: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
}
#endif