#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include<algorithm>
#include<fstream>
#include<ios>

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
};

struct Coord2 {
	num x, y;
};

typedef std::vector<Coord2> CoordVec;

struct Node {
	Coord2 TopLeft;
	num Width, Height;
	BitColor Color;
};

class Image {
private:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	std::ifstream fstream;
	std::vector< std::vector<BitColor> > Bitmap;

	Coord2 MaxPoint;
	CoordVec Nodes; //Indicate left-bottom's position
	CoordVec Edges;
public:
	Image() : fstream() {}
	Image(string filename, std::_Iosb<int>::_Openmode mod) { fstream.open(filename, mod); ReadFile(); }
	Image(string filename) : Image(filename, std::ios::in) {}
	void ReadFile();
};

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

	const int width = infoHeader.biWidth;
	const int height = infoHeader.biHeight;
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
	for (int y = height - 1; y >= 0; y--){
		for (int x = 0; x < width; x++){
			fstream >> tmp.B >> tmp.G >> tmp.R;
			Bitmap[y][x] = tmp;
		}
	}
}