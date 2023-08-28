#include <iostream>
#include <memory>

#include "Window.h"

int main()
{
    try
    {
        auto window = std::make_unique<Window>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }

    return 0;
}
