#pragma once
#include "../include/bmp.hpp"

struct YUVImage {
	std::uint32_t width;
	std::uint32_t height;
	std::vector<std::uint8_t> yPlane;
	std::vector<std::uint8_t> uPlane;
	std::vector<std::uint8_t> vPlane;
};

// ColorClamp
std::uint8_t clrClamp(float val);


// Convert rows
void RgbToYuvRows(const Bitmap& bmp, YUVImage& yuv, int startRow, int endRow);


// Convert rows with SSE
void RgbToYuvRowsSSE(const Bitmap& bmp, YUVImage& yuv, int startRow, int endRow);


// Convert Bitmap to YUVImage ( default version )
YUVImage RgbToYuv(const Bitmap& bmp);


// Convert Bitmap to YUVImage ( multithread version )
YUVImage RgbToYuvMT(const Bitmap& bmp);


// Overlay Image
bool overlayImage(
	const std::string& inputFile, 
	std::uint32_t videoWidth, 
	std::uint32_t videoHeight,
	const std::string& outputFile, 
	const Bitmap& bmp
);
