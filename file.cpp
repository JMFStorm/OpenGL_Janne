#include "file.h"

std::string readFileToString(const char* filePath)
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
