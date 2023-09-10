#include <filesystem>
#include <iostream>
#include <memory>
#include <nfd.h>

#include "Window.h"

int main(int argc, char *argv[])
{
    // Read the args (minus the program) from argv
    std::vector<std::string> args{argv + 1, argv + argc};

    NFD_Init();

    try
    {
        auto window = std::make_unique<Window>(argv[0]);

        // Parse command arguments
        if (!args.empty())
        {
            // Check if the first argument is a .txt
            if (std::filesystem::path(args[0]).extension() == ".txt")
            {
                window->OpenMaze(args[0]);
            }
        }

        window->Run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }

    NFD_Quit();

    return 0;
}
