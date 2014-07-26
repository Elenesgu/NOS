#include<iostream>
#include<string>

using std::endl;
using std::string;

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
	Coord2() :Coord2(0, 0) {}

	bool operator ==(const Coord2& obj) const{
		return (x == obj.x) && (y == obj.y);
	}
	bool operator !=(const Coord2& obj) const {
		return !(obj == *this);
	}
};

class Image {
private:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	FILE* fstream;
	std::ofstream output;
	int width, height;

	BitColor** Bitmap;

	static BitColor WhiteBit;
	static BitColor BlackBit;
public:
	Image() : fstream() {}
	Image(string filename, string mod) {
		fopen_s(&fstream, filename.c_str(), mod.c_str());
		ReadFile();
#ifdef _W_FILE
		output.open("output.txt", std::ios::out);
		WriteFile("output.bmp");
#endif

	}
	Image(string filename) : Image(filename, "rb") {}
	void ReadFile();
#ifdef _W_FILE
	void WriteFile(string filename);
#endif
	~Image() {
		fclose(fstream);
#ifdef _LOCAL
		int a;
		std::cin >> a;
#endif
#ifdef _W_FILE
		output.close();
#endif
	}
};

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
	Bitmap = (BitColor**)malloc(sizeof(BitColor*)* height);
	int trash;
	const int padding = width % 4;

#ifdef _LOCAL
	tend = NowTime();
	out << "Allocation End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	for (int y = height - 1; y >= 0; y--) {
		Bitmap[y] = (BitColor*)malloc(sizeof(BitColor)* width);
		fread(reinterpret_cast<char *>(Bitmap[y]), sizeof(BitColor)* width, 1, fstream);
		fread(reinterpret_cast<char *>(&trash), sizeof(char)* padding, 1, fstream);

	}
#ifdef _LOCAL
	tend = NowTime();
	out << "File Read End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
}
