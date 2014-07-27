#include<iostream>
#include<string>
#include<vector>


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
#include<fstream>
std::ofstream out("output.txt");
#endif


typedef unsigned char byte;
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

struct BitPallet {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char Reserved;
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

struct Unit {
	Coord2 coord;
	float energy;
	Unit(Coord2 ac, float ae): coord(ac), energy(ae) {}
	Unit(int ax, int ay, float ae): coord(ax, ay), energy(ae) {}
	Unit(int ax, int ay) : Unit(ax, ay, 0) {}
	Unit(Coord2 ac) : Unit(ac, 0) {}
	Unit() : Unit(-1, -1){}
};

class Image {
private:
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	FILE* pImageFile;
	FILE* pTextFile;
	int width, height;
	BitPallet Pallet[256];

	byte** Bitmap;

	std::vector<Unit> UFO;
	std::vector<Unit> Bunker;

	byte BlackBit;
public:
	Image() : pImageFile() {}
	Image(string imagename, string imod, string txtname, string txtmod) {
		fopen_s(&pImageFile, imagename.c_str(), imod.c_str());
		fopen_s(&pTextFile, txtname.c_str(), txtmod.c_str());
		ReadImage();
		ReadText();
#ifdef _W_FILE
		output.open("output.txt", std::ios::out);
		WriteFile("output.bmp");
#endif

	}
	Image(string Iname, string Tname) : Image(Iname, "rb", Tname, "rb") {}
	void ReadImage();
	void ReadText();
#ifdef _W_FILE
	void WriteFile(string filename);
#endif
	~Image() {
		fclose(pImageFile);
		fclose(pTextFile);
		for (int i = 0; i < height; i++){
			free(Bitmap[i]);
		}
		free(Bitmap);
#ifdef _LOCAL
		/*
		int a;
		std::cin >> a;
		*/
#endif
#ifdef _W_FILE
		output.close();
#endif
	}
};

int main() {
	string IimageName, Itxtname;

#ifdef _LOCAL
	tstart = NowTime();
	/*
	out << "Input World File Name(not include .bmp): ";
	std::cin >> IimageName;
	IimageName += ".bmp";
	out << "Input Text File Name(not include .txt): ";
	std::cin >> Itxtname;
	Itxtname += ".txt";
	*/
	IimageName = "world.bmp";
	Itxtname = "input.txt";
#else
	IimageName = "world.bmp";
	Itxtname = "input.txt";
#endif

	Image Data(IimageName, Itxtname);

#ifdef _LOCAL
	tend = NowTime();
	out << "Total Operating time: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
	return 0;
}

void Image::ReadImage() {
	if (!pImageFile){
		out << "File not found" << endl;
		exit(-1);
	}
	fread(reinterpret_cast<char*>(&fileHeader), sizeof fileHeader, 1, pImageFile);
	if (fileHeader.bfType != ('B' | (static_cast<int>('M') << 8))) {
		out << "File is not a bitmap" << endl;
		return;
	}
	fread(reinterpret_cast<char*>(&infoHeader), sizeof infoHeader, 1, pImageFile);

	width = infoHeader.biWidth;
	height = infoHeader.biHeight;
	const int bitsPerPixel = infoHeader.biBitCount;
	const int bytesPerPixel = bitsPerPixel / 8;
	const int pitch = (width * bytesPerPixel + 3) % 4;
	const int dataSize = pitch * height;
	int padding;

	if (	infoHeader.biPlanes != 1 ||
		infoHeader.biCompression != 0
		) {
		out << "Unsupported Format" << endl;
		return;
	}

	fread(reinterpret_cast<char*>(Pallet), sizeof(BitPallet)* 256, 1, pImageFile);

	const int dataBeginPos = static_cast<int> (ftell(pImageFile));

	if (dataBeginPos != static_cast<int>(fileHeader.bfOffBits))
	{
		out << "Header has not read completely" << endl;
		return;
	}

	for (int i = 0; i < 256; i++) {
		if (Pallet[i].B == 0 && Pallet[i].G == 0 && Pallet[i].R == 0) {
			BlackBit = i;
			break;
		}
	}

	Bitmap = (byte**)malloc(sizeof(byte*)* height);

#ifdef _LOCAL
	tend = NowTime();
	out << "Allocation and Head Reading End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif

	for (int y = height - 1; y >= 0; y--) {
		Bitmap[y] = (byte*)malloc(sizeof(byte)* width);
		fread(reinterpret_cast<char*>(Bitmap[y]), sizeof(byte)* width, 1, pImageFile);
		fread(reinterpret_cast<char*>(&padding), sizeof(byte)* pitch, 1, pImageFile);
	}
	
#ifdef _LOCAL
	tend = NowTime();
	out << "Image File Read End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
}

void Image::ReadText() {
	if (!pTextFile){
		out << "Text File not found" << endl;
		exit(-1);
	}
	num uCase, bCase;
	num x, y;
	float energy;
	fscanf_s(pTextFile, "%d", &uCase);
	UFO.reserve(uCase);
	while (uCase--) {
		fscanf_s(pTextFile, "%d", &x);
		fscanf_s(pTextFile, "%d", &y);
		fscanf_s(pTextFile, "%g", &energy);
		UFO.push_back(Unit(x, y, energy));
	}

	fscanf_s(pTextFile, "%d", &bCase);

	Bunker.reserve(bCase);
	while (bCase--) {
		fscanf_s(pTextFile, "%d", &x);
		fscanf_s(pTextFile, "%d", &y);	
		Bunker.push_back(Unit(x, y));
	}

#ifdef _LOCAL
	tend = NowTime();
	out << "Text File Read End: " << std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart).count() << "ms" << endl;
#endif
}
