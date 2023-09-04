#include <iostream>
#include <memory>
#include <nfd.h>

#include "Window.h"

int main()
{
    NFD_Init();

    try
    {
        auto window = std::make_unique<Window>();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }

    NFD_Quit();

    return 0;
}
