#pragma once

#include <string>

std::string ReadFileToString(std::string);

unsigned char* LoadImage(const char* filePath, int* width, int* height, int* nrChannels);

void FreeImageData(unsigned char* data);
