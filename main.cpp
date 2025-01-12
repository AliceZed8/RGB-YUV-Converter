#include "include/bmp.hpp"
#include "include/yuv.hpp"
#include <iostream>


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




