
#include <cassert>
#include <cstdio>
#include <vector>


#pragma pack(push, 1)

struct BitmapFileHeader
{
	unsigned short bfType;
	unsigned int   bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int   bfOffBits;
};

struct BitmapInfoHeader
{
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


struct ScopedFilePointer
{
	ScopedFilePointer(const char* filename, const char* mode) : fp_(NULL)
	{
		fp_ = fopen(filename, mode);
	}
	~ScopedFilePointer()
	{
		if (fp_)
		{
			fclose(fp_);
		}
	}
	operator FILE* () const { return fp_; }

private:
	FILE* fp_;
	void operator==(const ScopedFilePointer&);
	ScopedFilePointer(const ScopedFilePointer&);
};

class RawImage
{
public:
	RawImage();
	RawImage(int width, int height, int bytesPerPixel);

	int Width() const;
	int Height() const;
	int BytesPerPixel() const;
	const void* Bits() const;
	void* MutableBits();

	void Teardown();
	void Allocate(int width, int height, int bytesPerPixel);

	void Swap(RawImage& other);

private:
	int width;
	int height;
	int bytesPerPixel;
	std::vector<char> data;
};

RawImage::RawImage() : width(0), height(0), bytesPerPixel(0)
{
}

RawImage::RawImage(int width, int height, int bytesPerPixel)
	: width(0), height(0), bytesPerPixel(0)
{
	Allocate(width, height, bytesPerPixel);
}

int RawImage::Width() const
{
	return width;
}

int RawImage::Height() const
{
	return height;
}

int RawImage::BytesPerPixel() const
{
	return bytesPerPixel;
}

const void* RawImage::Bits() const
{
	if (data.empty())
	{
		return 0;
	}
	else
	{
		return &data[0];
	}
}

void* RawImage::MutableBits()
{
	return const_cast<void*>(Bits());
}

void RawImage::Teardown()
{
	width = 0;
	height = 0;
	bytesPerPixel = 0;
	data.clear();
}

void RawImage::Allocate(int width, int height, int bytesPerPixel)
{
	assert(width >= 0 && height >= 0 && bytesPerPixel >= 0);
	const int bytes = width * height * bytesPerPixel;
	data.resize(bytes);
	this->width = width;
	this->height = height;
	this->bytesPerPixel = bytesPerPixel;
}

void RawImage::Swap(RawImage& other)
{
	std::swap(width, other.width);
	std::swap(height, other.height);
	std::swap(bytesPerPixel, other.bytesPerPixel);
	data.swap(other.data);
}

bool SaveRawImageToFile(const RawImage& img, const char* filename)
{
	const int w = img.Width();
	const int h = img.Height();
	const int bpp = img.BytesPerPixel();

	const ScopedFilePointer fp(filename, "wb");
	if (!fp)
	{
		return false;
	}

	const void* p = img.Bits();
	if (!p)
	{
		return false;
	}

	fwrite(p, w * h * bpp, 1, fp);
	return true;

}

bool RawImageFromBmpFile(const char* filename, RawImage& result)
{
	result.Teardown();

	if (!filename || filename[0] == '\0')
	{
		fprintf(stderr, "filename required!\n", filename);
		return false;
	}
	const ScopedFilePointer fp(filename, "rb");

	if (!fp)
	{
		fprintf(stderr, "Could not open file \"%s\"!\n", filename);
		return false;
	}

	BitmapFileHeader fileHeader = {};
	fread(&fileHeader, sizeof(fileHeader), 1, fp);
	if (fileHeader.bfType != ('B' | ((int)'M' << 8)))
	{
		fprintf(stderr, "\"%s\" is not a bitmap file!\n", filename);
		return false;
	}

	BitmapInfoHeader infoHeader = {};
	fread(&infoHeader, sizeof(infoHeader), 1, fp);

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
		)
	{
		fprintf(stderr, "\"%s\" is unsupported format!\n", filename);
		fprintf(stderr,
			"  Width:%d, Height:%d, Bpp:%d, Plane:%d, "
			"Compression:%d, Size:%d\n",
			width, height, infoHeader.biBitCount, infoHeader.biPlanes,
			infoHeader.biCompression, infoHeader.biSizeImage);
		return false;
	}

	const int dataBeginPos = ftell(fp);
	if (dataBeginPos != static_cast<int>(fileHeader.bfOffBits))
	{
		fprintf(stderr, "\"%s\" is unsupported format!\n", filename);
		fprintf(stderr, "  OffsetBits:%d\n", fileHeader.bfOffBits);
		return false;
	}

	result.Allocate(width, height, bytesPerPixel);
	char* const destBegin = static_cast<char*>(result.MutableBits());

	const int destPitch = width * bytesPerPixel;
	const int gap = pitch - destPitch;
	char* curDest = destBegin + height * destPitch;
	for (int y = height; y > 0; --y)
	{
		curDest -= destPitch;
		fread(curDest, destPitch, 1, fp);
		if (gap > 0)
		{
			fseek(fp, gap, SEEK_CUR);
		}
		for (int x = 0; x < width; ++x)
		{
			char temp = curDest[x * 3];
			curDest[x * 3] = curDest[x * 3 + 2];
			curDest[x * 3 + 2] = temp;
		}
	}

	const int expectedEnd =
		fileHeader.bfOffBits + infoHeader.biSizeImage;
	const int dataEndPos = ftell(fp);
	return dataEndPos == expectedEnd;
}


int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		printf("bmp2raw <input-filename> [output-filename]\n");
		return 0;
	}

	const char* inputFilename = "input.bmp";
	const char* outputFilename = "output.data";
	if (argc > 1)
	{
		inputFilename = argv[1];
	}
	if (argc > 2)
	{
		outputFilename = argv[2];
	}

	RawImage img;
	RawImageFromBmpFile(inputFilename, img);
	SaveRawImageToFile(img, outputFilename);

	return 0;
}
