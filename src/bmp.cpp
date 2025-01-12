#include "../include/bmp.hpp"
#include <iostream>
#include <fstream>

// Read Bitmap from file
bool Bitmap::read(const std::string& filename) {
	// Clear data
	fileHeader = {};
	infoHeader = {};
	data.clear();
	
	std::ifstream in(filename, std::ios::binary);
	if (!in) {
		std::cerr << "Cannot open file " << filename << std::endl;
		return false;	
	}
		
	in.read((char*) &fileHeader, sizeof(fileHeader));
	if (fileHeader.bfType != 0x4D42) { // "BM"
		std::cerr << "Failed to read, not bmp file " << filename << std::endl;
		return false;
	}
		
	in.read((char*) &infoHeader, sizeof(infoHeader));
	int width = infoHeader.width;
	int height = infoHeader.height;
	
	// Get image size
	std::size_t size;
	if (infoHeader.sizeImage == 0) {
		size = width * 3 + width % 4;
		size *= height;
	} else size = infoHeader.sizeImage;
	
	data.resize(size);
	in.seekg(fileHeader.bfOffBits, in.beg);
	in.read((char*) data.data(), size);

	return true;
}
