#include "spritebitmappacket.h"
#include "bitmap.h"
#include "output.h"
#include <vector>

#define w 72
#define h 128
#define frames 3
#define facings 4
#define frame_w 24
#define frame_h 32

void FrameLocal(unsigned int x, unsigned int y, unsigned int& lx, unsigned int& ly) 
{
	lx = x % (frame_w);
	ly = y % (frame_h);
}

/*
Crop:
2 bytes
4 bits - left crop
4 bits - top crop
4 bits - right crop
4 bits - bottom crop

Palette size:
1 byte - size 0 - 255

Palette:
size * 4 bytes (0 - 1024): colors

change mask size is deduced from crop

change masks
01
02
10
11
12
20
21
22
f0
f1
f2

change lengths
11 bytes

changes

*/

class BitBuffer 
{
	private:
	unsigned char* buffer;
	unsigned int byteSize;
	public:
	BitBuffer(unsigned int bits) 
	{
		byteSize = bits / 8 + ((bits % 8) ? 1 : 0);
		buffer = new unsigned char[byteSize];
		memset(buffer, 0, byteSize);
	}

	~BitBuffer() 
	{
		//delete[] buffer;
	}

	bool get(unsigned int bit) 
	{
		return buffer[bit / 8] & (1 << (bit % 8));
	}

	bool get(unsigned int x, unsigned int y, unsigned int width) 
	{
		return get(x + y * width);
	}

	void set(unsigned int bit, bool state) 
	{
		if(bit / 8 > byteSize) Output::Error("Out of bounds {} out of {}", bit, byteSize * 8);
		buffer[bit / 8] = buffer[bit / 8] | (1 << (bit % 8));
	}

	void set(unsigned int x, unsigned int y, unsigned int width, bool state) 
	{

		if((x + y * width) / 8 > byteSize) Output::Debug("Out of bounds ({}; {}), w: {} out of {}", x, y, width, byteSize * 8);
		set(x + y * width, state);
	}

	unsigned char* getBuffer() 
	{
		return buffer;
	}

	unsigned int getByteSize() 
	{
		return byteSize;
	}
};

class PackBuffer : public BitBuffer 
{
	private:
	int bpe;
	public:
	int pushCursor;
	PackBuffer(int bitsPerElement, int elements) : BitBuffer(bitsPerElement * elements), bpe(bitsPerElement)
	{
		pushCursor = 0;
	}

	void write(unsigned int index, unsigned int value) 
	{
		for(int i = 0; i < bpe; i++) 
		{
			set(index * bpe + i, (value >> i) % 2);
		}
	}

	int read(unsigned int index) 
	{
		int value = 0;
		for(int i = 0; i < bpe; i++) 
		{
			value += get(index * bpe + i) << i;
		}
		return value;
	}

	void push(unsigned int value) 
	{
		write(pushCursor++, value);
	}

	unsigned int pushedByteSize() 
	{
		return pushCursor * bpe / 8 + (((pushCursor * bpe) % 8) ? 1 : 0);
	}
};

struct Palette 
{
	unsigned int palette[256];
	unsigned int paletteSize;

	Palette() 
	{
		paletteSize = 0;
	}

	void add(unsigned int color32) 
	{
		if(paletteSize == 256) 
		{
			Output::Error("Too many colors for a sprite bitmap packet");
		}
		for(int i = 0; i < paletteSize; i++) 
		{
			if(palette[i] == color32) 
			{
				return;
			}
		}
		
		palette[paletteSize++] = color32;
	}

	unsigned char get(unsigned int color32) 
	{
		for(int i = 0; i < paletteSize; i++) 
		{
			if(palette[i] == color32) 
			{
				return i;
			}
		}

		Output::Error("Couldn't find palette color");
		return -1;
	}
};

BitBuffer* RLEncode(BitBuffer& bitbuffer) 
{
	unsigned char count[2] = {0,0};
	std::vector<unsigned char> counts;
	for(int i = 0; i < bitbuffer.getByteSize() * 8; i++) 
	{
		bool bit = bitbuffer.get(i);
		count[bit]++;
		if(count[!bit]) {
			counts.push_back(count[!bit] - 1);
			count[!bit] = 0;
		}
		if(count[bit] == 16) 
		{
			counts.push_back(count[bit] - 1);
			count[bit] = 0;
		}
	}

	BitBuffer* rlebuffer = new BitBuffer(counts.size()*4);
	for(int i = 0; i < counts.size(); i++) 
	{	
		rlebuffer->set(i * 4 + 0, counts[i] & (1 << 0));
		rlebuffer->set(i * 4 + 1, counts[i] & (1 << 1));
		rlebuffer->set(i * 4 + 2, counts[i] & (1 << 2));
		rlebuffer->set(i * 4 + 3, counts[i] & (1 << 3));
	}

	return rlebuffer;
}

#define pixel(x, y) pixels[x + y * w]

void UnpackBitmap(unsigned char* packedBitmap);

OSpriteBitmapPacket::OSpriteBitmapPacket(BitmapRef& bitmap) 
{
	if(bitmap->GetWidth() != w || bitmap->GetHeight() != h) 
	{
		Output::Error("Invalid bitmap dimensions for sprite bitmap packet");
	}

	Palette palette;

	unsigned int bitmapCopy[w][h];
	unsigned char r, g, b, a;
	unsigned int row, column, i = 0;
	unsigned int 
	left = 32,
	top = 32,
	right = 32,
	bottom = 32;
	unsigned char ar, ag, ab, aa;
	unsigned int* pixels = (unsigned int*)bitmap->pixels();

	bitmap->pixel_format.uint32_to_rgba(pixels[0], ar, ag, ab, aa);

	for(row = 0; row < h; row++) 
	{
		for(column = 0; column < w; column++) 
		{
			bitmap->pixel_format.uint32_to_rgba(pixels[i], r, g, b, a);
			//Output::Debug("Pixel ({}, {}), rgba({}, {}, {}, {})", column, row, r, g, b, a);
			palette.add(pixels[i]);
			if(ar != r || ag != g || ab != b) 
			{
				unsigned int lx, ly;
				FrameLocal(column, row, lx, ly);
				//crop frames
				left = std::min(lx, left);
				top = std::min(ly, top);
				right = std::min(frame_w - lx - 1, right);
				bottom = std::min(frame_h - ly - 1, bottom);
			}
			i++;
		}
	}

	std::string paletteStr = "";
	for(int j = 0; j < palette.paletteSize; j++) 
	{
		paletteStr += std::to_string(palette.palette[j]) + " ";
	}

	Output::DebugStr(paletteStr);

	unsigned int croppedFrameSize = (frame_w - left - right) * (frame_h - top - bottom);
	unsigned int croppedFrameWidth = frame_w - left - right;

	Output::Debug("Croped bitmap size: ({}, {}), left: {}, top: {}, right: {}, bottom: {}", croppedFrameWidth, frame_h - top - bottom, left, top, right, bottom);

	BitBuffer changeMask01(croppedFrameSize);
	BitBuffer changeMask02(croppedFrameSize);
	BitBuffer changeMask10(croppedFrameSize);
	BitBuffer changeMask11(croppedFrameSize);
	BitBuffer changeMask12(croppedFrameSize);
	BitBuffer changeMask20(croppedFrameSize);
	BitBuffer changeMask21(croppedFrameSize);
	BitBuffer changeMask22(croppedFrameSize);
	BitBuffer changeMaskF0(croppedFrameSize);
	BitBuffer changeMaskF1(croppedFrameSize);
	BitBuffer changeMaskF2(croppedFrameSize);
	
	int bpe = 1;
	int psize = palette.paletteSize;
	while(psize) 
	{
		psize = psize >> 1;
		bpe++;
	}

	PackBuffer changes01(bpe, 512);
	PackBuffer changes02(bpe, 512);
	PackBuffer changes10(bpe, 512);
	PackBuffer changes11(bpe, 512);
	PackBuffer changes12(bpe, 512);
	PackBuffer changes20(bpe, 512);
	PackBuffer changes21(bpe, 512);
	PackBuffer changes22(bpe, 512);
	PackBuffer changesF0(bpe, 512);
	PackBuffer changesF1(bpe, 512);
	PackBuffer changesF2(bpe, 512);

	i = 0;
	int croppedX = 0;
	int croppedY = 0;
	
	for(row = top; row < frame_h - bottom; row++) 
	{
		croppedX = 0;
		for(column = top; column < frame_w - right; column++) 
		{
			int p00 = pixel(column, row);
			int p01 = pixel(column + frame_w, row);
			int p02 = pixel(column + frame_w * 2, row);
			int p10 = pixel(column, row + frame_h);
			int p11 = pixel(column + frame_w, row + frame_h);
			int p12 = pixel(column + frame_w * 2, row + frame_h);
			int p20 = pixel(column, row + frame_h * 2);
			int p21 = pixel(column + frame_w, row + frame_h * 2);
			int p22 = pixel(column + frame_w * 2, row + frame_h * 2);
			int pF0 = pixel(frame_w - column - 1, row + frame_h * 3);
			int pF1 = pixel(frame_w * 2 - column - 1, row + frame_h * 3);
			int pF2 = pixel(frame_w * 3 - column - 1, row + frame_h * 3);
			
			//back to anim
			if(p00 != p01) {
				changeMask01.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes01.push(palette.get(p01));
			}
			if(p00 != p02) {
				changeMask02.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes02.push(palette.get(p02));
			}

			//back to right
			if(p00 != p10) {
				changeMask10.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes10.push(palette.get(p10));
			}

			//right to anim
			if(p10 != p11) {
				changeMask11.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes11.push(palette.get(p11));
			}
			if(p10 != p12) {
				changeMask12.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes12.push(palette.get(p12));
			}

			//right to front
			if(p10 != p20) {
				changeMask20.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes20.push(palette.get(p20));
			}

			//front to anim
			if(p20 != p21) {
				changeMask21.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes21.push(palette.get(p21));
			}
			if(p20 != p22) {
				changeMask22.set(croppedX, croppedY, croppedFrameWidth, 1);
				changes22.push(palette.get(p22));
			}

			//right to left flipped
			if(p10 != pF0) {
				changeMaskF0.set(croppedX, croppedY, croppedFrameWidth, 1);
				changesF0.push(palette.get(pF0));
			}
			if(p11 != pF1) {
				changeMaskF1.set(croppedX, croppedY, croppedFrameWidth, 1);
				changesF1.push(palette.get(pF1));
			}
			if(p12 != pF2) {
				changeMaskF2.set(croppedX, croppedY, croppedFrameWidth, 1);
				changesF2.push(palette.get(pF2));
			}
			
			i++;
			croppedX++;
		}
		croppedY++;
	}

	this->packedBitmap[0] = left + (top << 4);
	this->packedBitmap[1] = right + (bottom << 4);
	
	this->packedBitmap[2] = palette.paletteSize;
	int p;
	for(p = 0; p < palette.paletteSize; p++) 
	{
		((unsigned int*)&packedBitmap[3])[p] = palette.palette[p];
	}

	p = 3 + palette.paletteSize * 4;
	Output::Debug("Palette with {} colors encoded in {} bytes", palette.paletteSize, p - 4);

	#define addChangemask(ii)\
	BitBuffer* rle##ii = RLEncode(changeMask##ii);\
	memcpy(&packedBitmap[p], rle##ii->getBuffer(), rle##ii->getByteSize());\
	p += rle##ii->getByteSize()

	addChangemask(01);
	addChangemask(02);
	addChangemask(10);
	addChangemask(11);
	addChangemask(12);
	addChangemask(20);
	addChangemask(21);
	addChangemask(22);
	addChangemask(F0);
	addChangemask(F1);
	addChangemask(F2);
	
	#define addChanges(ii) \
	p++; \
	memcpy(&packedBitmap[p], changes##ii.getBuffer(), changes##ii.pushedByteSize()); \
	p += changes##ii.pushedByteSize()

	addChanges(01);
	addChanges(02);
	addChanges(10);
	addChanges(11);
	addChanges(12);
	addChanges(20);
	addChanges(21);
	addChanges(22);
	addChanges(F0);
	addChanges(F1);
	addChanges(F2);

	Output::Debug("Compressed bitmap to {} bytes", p);
	UnpackBitmap(packedBitmap);
}

void UnpackBitmap(unsigned char* packedBitmap) 
{
	int left, top, right, bottom;
	left = packedBitmap[0] % (1 << 4);
	top = packedBitmap[0] >> 4;
	right = packedBitmap[1] % (1 << 4);
	bottom = packedBitmap[1] >> 4;

	int paletteSize = packedBitmap[2];
	unsigned int palette[256];

	for(int i = 0; i < paletteSize; i++) 
	{
		palette[i] = *((unsigned int*)(packedBitmap + 3 + i * 4));
	}
	
	std::string paletteStr = "";
	for(int j = 0; j < paletteSize; j++) 
	{
		paletteStr += std::to_string(palette[j]) + " ";
	}

	Output::DebugStr(paletteStr);

	Output::Debug("left: {}, top: {}, right: {}, bottom: {}, palette: {}; ", left, top, right, bottom, paletteSize);
}