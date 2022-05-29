#pragma once

#include <string>

std::string readFileToString(std::string);

unsigned char* loadImage(const char* filePath, int* width, int* height, int* nrChannels);

void freeImageData(unsigned char* data);
