#include "../include/yuv.hpp"
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

// ColorClamp (simple std::clamp implementation)
std::uint8_t clrClamp(float val) {
	return val <= 0 ? 0 : val >= 255 ? 255 : val;
}

// Convert rows (from start to end) with flip 
void RgbToYuvRows(const Bitmap& bmp, YUVImage& yuv, int startRow, int endRow) {
	int width = bmp.infoHeader.width;
	int height = bmp.infoHeader.height;
	
	for (int j = startRow; j < endRow; j++) {
		for (int i = 0; i < width; i++) {
			int rgbIndex = (j*width + i) * 3;
			int yIndex = (height - 1 - j)*width + i; // flip
			int uvIndex = ((height - 1 - j)/2) * (width/2) + (i/2);
			
			// Get B G R
			std::uint8_t b = bmp.data[rgbIndex];
			std::uint8_t g = bmp.data[rgbIndex + 1];
			std::uint8_t r = bmp.data[rgbIndex + 2];
			
			// Convert to YUV
			yuv.yPlane[yIndex] = clrClamp(0.299*r + 0.587*g + 0.114*b + 16);
			if ((i % 2 == 0) && ((height - 1 - j) % 2 == 0)) {
				yuv.uPlane[uvIndex] = clrClamp(-0.14713*r - 0.28886*g + 0.436*b + 128);
				yuv.vPlane[uvIndex] = clrClamp(0.615*r - 0.51499*g - 0.10001*b + 128);
			}
		}
	}
}


/* 
	Convert Bitmap to YUVImage ( default version ) 
*/
YUVImage RgbToYuv(const Bitmap& bmp) {
	YUVImage yuv;

	int width = bmp.infoHeader.width;
	int height = bmp.infoHeader.height;
	int frameSize = width * height;
	int chromaSize = frameSize / 4;

	yuv.width = width;
	yuv.height = height;
	yuv.yPlane.resize(frameSize);
	yuv.uPlane.resize(chromaSize);
	yuv.vPlane.resize(chromaSize);
	
	RgbToYuvRows(bmp, yuv, 0, height);	
	return yuv;
}


/*
	Convert Bitmap to YUVImage ( multithread version )
*/

YUVImage RgbToYuvMT(const Bitmap& bmp) {
	YUVImage yuv;
	int width = bmp.infoHeader.width;
	int height = bmp.infoHeader.height;
	int frameSize = width * height;
	int chromaSize = frameSize / 4;

	yuv.width = width;
	yuv.height = height;
	yuv.yPlane.resize(frameSize);
	yuv.uPlane.resize(chromaSize);
	yuv.vPlane.resize(chromaSize);
	

	int threadsNum = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	int rowsPerThread = height / threadsNum;

	for (int i = 0; i < threadsNum; ++i) {
		int startRow = i*rowsPerThread;
		int endRow = (i == threadsNum - 1) ? height : startRow + rowsPerThread;
		threads.emplace_back(RgbToYuvRows, std::ref(bmp), std::ref(yuv), startRow, endRow);
	}

	for (std::thread& t: threads) {
		t.join();
	}

	return yuv;
}





/*
	Overlay Image 
*/
bool overlayImage(
		const std::string& inputFile, 
		std::uint32_t videoWidth, 
		std::uint32_t videoHeight,
		const std::string& outputFile, 
		const Bitmap& bmp
	) {
	// Check size
	if (bmp.infoHeader.width > videoWidth || bmp.infoHeader.height > videoHeight) {
		std::cerr << "Invalid Bmp image size" << std::endl;
		return false;
	}

	// in, out streams
	std::ifstream in(inputFile, std::ios::binary);
	std::ofstream out(outputFile, std::ios::binary);
	if (!in) {
		std::cerr << "Failed to open input file " << inputFile << std::endl;
		return false;
	}
	if (!out) {
		std::cerr << "Failed to open output file " << outputFile << std::endl;
		return false;
	}
	
	// YUVImage converted = RgbToYuv(bmp);
	YUVImage converted = RgbToYuvMT(bmp);

	std::uint32_t frameSize = videoWidth * videoHeight;
	std::uint32_t chromaSize = frameSize / 4;

	// Frame
	YUVImage frame {videoWidth, videoHeight};
	frame.yPlane.resize(frameSize);
	frame.uPlane.resize(chromaSize);
	frame.vPlane.resize(chromaSize);

	// Read frames
	while (
		in.read((char*) frame.yPlane.data(), frameSize) &&
		in.read((char*) frame.uPlane.data(), chromaSize) &&
		in.read((char*) frame.vPlane.data(), chromaSize) ) 
	{
		// overlay frame
		for (int j = 0; j < converted.height; ++j) {
			for (int i = 0; i < converted.width; ++i) {
				int frameYIndex = j * frame.width + i;
				int convYIndex = j * converted.width + i;

				frame.yPlane[frameYIndex] = converted.yPlane[convYIndex];
				if (i % 2 == 0 && j % 2 == 0) {
					int frameUVIndex = (j/2) * (frame.width/2) + (i/2);
					int convUVIndex = (j/2) * (converted.width/2) + (i/2);
					frame.uPlane[frameUVIndex] = converted.uPlane[convUVIndex];
					frame.vPlane[frameUVIndex] = converted.vPlane[convUVIndex];
				}
			}
		}

		// Write frame
		out.write((char*) frame.yPlane.data(), frameSize);
		out.write((char*) frame.uPlane.data(), chromaSize);
		out.write((char*) frame.vPlane.data(), chromaSize);
	}


	return true;
}
