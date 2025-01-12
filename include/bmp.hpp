#pragma once
#include <cstdint>
#include <string>
#include <vector>

#pragma pack(push, 1)
struct BitmapFileHeader {
	std::uint16_t bfType		= 0x4D42;	// format type
	std::uint32_t bfSize		= 0;		// file size (in bytes)
	std::uint16_t bfReserved1	= 0;		// reserved (always 0)
	std::uint16_t bfReserved2	= 0;
	std::uint32_t bfOffBits		= 0;		// offset (start position of pixel data)
};


struct BitmapInfoHeader {
	std::uint32_t size			= 0; // size of this header (in bytes)
	std::int32_t width			= 0; // width, height of bitmap in pixels 
	std::int32_t height			= 0;

	std::uint16_t planes		= 1; // always 1
	std::uint16_t bitCount		= 0;
	std::uint32_t compression	= 0; // 0 if no compressed 
	std::uint32_t sizeImage		= 0; 
	std::int32_t xPelsPerMeter  = 0;
	std::int32_t yPelsPerMeter  = 0;
	
	std::uint32_t clrUsed		= 0;
	std::uint32_t clrImportant  = 0;

};
#pragma pack(pop)


struct Bitmap {
	bool read(const std::string& filename);
	
	BitmapFileHeader fileHeader;
	BitmapInfoHeader infoHeader;
	std::vector<std::uint8_t> data;
};






