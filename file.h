#pragma once

#include <string>
#include <fstream>
#include <sstream>

std::string readFileToString(const char* filePath);

unsigned char* loadImage(const char* filePath, int* width, int* height, int* nrChannels);

void freeImageData(unsigned char* data);