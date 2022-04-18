#include "file.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string readFileToString(std::string filePath)
{
    std::stringstream sStream;
    std::ifstream source;
    std::string result;

    // Ensure ifstream objects can throw exceptions:
    source.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        source.open(filePath);
        sStream << source.rdbuf();
        source.close();

        result = sStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        printf("File read failed: %s\n", e.what());
    }

    return result;
}

unsigned char* loadImage(const char* filePath, int* width, int* height, int* nrChannels)
{
    stbi_set_flip_vertically_on_load(true);
    return stbi_load(filePath, width, height, nrChannels, 0);
}

void freeImageData(unsigned char* data)
{
    stbi_image_free(data);
}