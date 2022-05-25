#include "jUtil.h"

#include <iostream>

void jAssert(bool assertion, std::string errorMessage)
{
    if (assertion == false)
    {
        std::string text = errorMessage;
        std::cout << text << std::endl;
        abort();
    }
}
