#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>
#include "include/bmp.hpp"

struct YUVImage {
	std::uint32_t width;
	std::uint32_t height;
	std::vector<std::uint8_t> yPlane;
	std::vector<std::uint8_t> uPlane;
	std::vector<std::uint8_t> vPlane;
};


std::uint8_t clrClamp(float val) {
	return val <= 0 ? 0 : val >= 255 ? 255 : val;
}

// Convert Bitmap to YUVImage
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
	
	for (int j = 0; j < height; j++) {
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

	return yuv;
} 

// Overlay Image Video
void overlayImage(
		const std::string& inputFile, 
		std::uint32_t videoWidth, 
		std::uint32_t videoHeight,
		const std::string& outputFile, 
		const Bitmap& bmp
	) {
	// in, out streams
	std::ifstream in(inputFile, std::ios::binary);
	std::ofstream out(outputFile, std::ios::binary);
	if (!in) {
		std::cerr << "Failed to open input file " << inputFile << std::endl;
		return;
	}
	if (!out) {
		std::cerr << "Failed to open output file " << outputFile << std::endl;
		return;
	}

	YUVImage converted = RgbToYuv(bmp);

	std::uint32_t frameSize = videoWidth * videoHeight;
	std::uint32_t chromaSize = frameSize / 4;

	// Frame
	YUVImage frame {videoWidth, videoHeight};
	frame.yPlane.resize(frameSize);
	frame.uPlane.resize(chromaSize);
	frame.vPlane.resize(chromaSize);

	// Read frames
	int frameNum = 1;
	while (
			in.read((char*) frame.yPlane.data(), frameSize) &&
			in.read((char*) frame.uPlane.data(), chromaSize) &&
			in.read((char*) frame.vPlane.data(), chromaSize) ) {
		std::cout << "Proccess frame " << frameNum << std::endl;
		++frameNum;
		
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
}



int main(int argc, char ** argv) {
	std::string videoPath, bmpPath, outputPath;
	int width, height;
	
	// Get args
	if (argc <= 1) {
		std::cout << "Usage: converter VIDEO WIDTH HEIGHT BMP OUTPUT\n" 
			<< "Overlay BMP on VIDEO with video params WIDTHxHEIGHT. Output video file OUTPUT" << std::endl;
		return 1;
	} else if (argc == 6) {
		videoPath = argv[1];
		width = std::atoi(argv[2]);
		height = std::atoi(argv[3]);
		bmpPath = argv[4];
		outputPath = argv[5];
	} else {
		std::cout << "Failed to parse args" << std::endl;
		return 1;
	}
	
	// Read bmp
	Bitmap bmp;
	if (!bmp.read(bmpPath)) {
		std::cerr << "Failed to load bmp file" << std::endl;
		return 1;
	}
	
	// Proccess all frames
	overlayImage(videoPath, width, height, outputPath, bmp);
	return 0;
}




