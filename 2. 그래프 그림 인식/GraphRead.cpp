#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <ios>
#include <queue>

using std::string;
using std::endl;

#define _LOCAL

#ifdef _LOCAL
std::ostream& out = std::cout;
#elif
#include<fstream>
std::ofstream& out = std::out;
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

#pragma pack(pop)

struct BitColor {
	unsigned char R, G, B;
	BitColor(unsigned char aR, unsigned char aG, unsigned char aB) :R(aR), G(aB), B(aB) {}
	BitColor() : BitColor(0, 0, 0) {}
	BitColor(const BitColor& obj) : BitColor(obj.R, obj.G, obj.B) { }
	
	bool operator ==(const BitColor& obj) const{
		return (R == obj.R) && (G == obj.G) && (B == obj.B);
	}
	bool operator !=(const BitColor& obj) const {
		return !(obj == *this);
	}
};

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
};

struct Edge{
	Node* First, Second;
};

struct tmpEdge {
	Coord2 First, Second;
	tmpEdge(Coord2 aFirst,Coord2 aSecond) : First(aFirst), Second(aSecond) { }
	tmpEdge() : tmpEdge(Coord2(-1, -1), Coord2(-1, -1)) {}
};

class Image {
private:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	std::ifstream fstream;
	std::ofstream output;
	int width, height;

	std::vector< std::vector<BitColor> > Bitmap;
	std::vector< std::vector<bool> > Visited;

	static BitColor WhiteBit;
	static BitColor BlackBit;

	std::vector<Node> Nodes;
	std::vector<Edge> Edges;
	std::vector<tmpEdge> tmpEdges;

public:
	Image() : fstream() {}
	Image(string filename, std::_Iosb<int>::_Openmode mod) {
		fstream.open(filename, mod);
		output.open("output.txt", std::ios::out);
		ReadFile();
		Search();
#ifdef _LOCAL
		WriteFile("output.bmp");
#endif

	}
	Image(string filename) : Image(filename, std::ios::in) {}
	void ReadFile();
	void Search();
	Node FindNode(Coord2 tl);
	tmpEdge FindEdge(Coord2 tl);
#ifdef _LOCAL
	void WriteFile(string filename);
#endif
	~Image() {
		fstream.close();
#ifdef _LOCAL
		output.close();
#endif
	}
};

BitColor Image::WhiteBit = BitColor(255, 255, 255);
BitColor Image::BlackBit = BitColor(0, 0, 0);

int main() {
	Image Data("input.bmp");
	return 0;
}

void Image::ReadFile() {
	if (!fstream){
		out << "fpointer null" << endl;
		return;
	}
	fstream.read(reinterpret_cast<char*>(&fileHeader), sizeof fileHeader);
	if (fileHeader.bfType != ('B' | (static_cast<int>('M') << 8))) {
		out << "File is not a bitmap" << endl;
		return;
	}

	fstream.read(reinterpret_cast<char*>(&infoHeader), sizeof infoHeader);

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
	const int dataBeginPos = fstream.tellg();
	if (dataBeginPos != static_cast<int>(fileHeader.bfOffBits))
	{
		out << "Unsupported Format" << endl;
		return;
	}
	BitColor tmp;
	Bitmap = std::vector<std::vector<BitColor> >(height, std::vector<BitColor>(width, BitColor(255, 255, 255)));
	Visited = std::vector<std::vector<bool> >(height, std::vector<bool>(width, false));
	for (int y = height - 1; y >= 0; y--) {
		for (int x = 0; x < width; x++){
			fstream >> tmp.B >> tmp.G >> tmp.R;
			Bitmap[y][x] = tmp;
		}
	}
}

void Image::Search() {
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++) {
			if (Bitmap[y][x] == WhiteBit) {
				Visited[y][x] = true;
			}
			else {
				if (Bitmap[y][x] == BlackBit){
					//Edge

				}
				else {
					//Node
					Node tmpnode = FindNode(Coord2(x, y));
					if (tmpnode.TopLeft != Coord2(-1, -1)){
						Nodes.push_back(tmpnode);
					}					
				}
			}
		}
	}
}

Node Image::FindNode(Coord2 tl) {
	BitColor nColor = Bitmap[tl.y][tl.x];
	int initx = tl.x, inity = tl.y;
	for (; initx < width; initx++) {		
		if (Bitmap[inity][initx] != nColor) {
			initx;
			break;
		}
	}
	for (; inity < height; inity++){
		if (Bitmap[inity][initx] != nColor){
			inity;
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
	std::queue<Coord2> BFSqueue;
	BFSqueue.push(tl);
	std::vector<Coord2> earth;
	Coord2 Curcoord;
	while (BFSqueue.empty()){
		Curcoord = BFSqueue.front();
		BFSqueue.pop();
		if (Bitmap[Curcoord.y][Curcoord.x + 1] != WhiteBit) {
			if (Bitmap[Curcoord.y][Curcoord.x + 1] == BlackBit) {
				BFSqueue.push(Coord2(Curcoord.x + 1, Curcoord.y));
			}
			else {
				earth.push_back(Coord2(Curcoord.x + 1, Curcoord.y));
			}
		}
		if (Bitmap[Curcoord.y][Curcoord.x - 1] != WhiteBit) {
			if (Bitmap[Curcoord.y][Curcoord.x - 1] == BlackBit) {
				BFSqueue.push(Coord2(Curcoord.x - 1, Curcoord.y));
			}
			else {
				earth.push_back(Coord2(Curcoord.x - 1, Curcoord.y));
			}
		}
		if (Bitmap[Curcoord.y + 1][Curcoord.x] != WhiteBit) {
			if (Bitmap[Curcoord.y + 1][Curcoord.x] == BlackBit) {
				BFSqueue.push(Coord2(Curcoord.x, Curcoord.y + 1));
			}
			else {
				earth.push_back(Coord2(Curcoord.x, Curcoord.y + 1));
			}
		}
		if (Bitmap[Curcoord.y - 1][Curcoord.x] != WhiteBit) {
			if (Bitmap[Curcoord.y - 1][Curcoord.x] == BlackBit) {
				BFSqueue.push(Coord2(Curcoord.x, Curcoord.y - 1));
			}
			else {
				earth.push_back(Coord2(Curcoord.x, Curcoord.y - 1));
			}
		}
	}
	if (earth.size() != 2){
		out << "Invalid Edge Detected" << endl;
	}
	return tmpEdge(earth[0], earth[1]);
}

#ifdef _LOCAL
void Image::WriteFile(string filename) {
	std::ofstream output(filename, std::ios::out);

	output.write(reinterpret_cast<char*>(&fileHeader), sizeof fileHeader);
	output.write(reinterpret_cast<char*>(&infoHeader), sizeof infoHeader);

	BitColor tmp;
	for (int y = infoHeader.biHeight - 1; y >= 0; y--) {
		for (int x = 0; x < infoHeader.biWidth; x++) {
			tmp = Bitmap[y][x];
			output << tmp.B << tmp.G << tmp.R;
		}	
	}
}
#endif