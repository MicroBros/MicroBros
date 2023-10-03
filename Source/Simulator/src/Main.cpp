#include <nfd.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "Window.h"

#include "BLE.h"

int main(int argc, char *argv[])
{
#ifdef WIN32
    // BLE needs to be initialised once here, or issues will arrise with WinRT, THANKS MICROSOFT
    Simulator::BLE ble{};
#endif

    // Read the args (minus the program) from argv
    std::vector<std::string> args{argv + 1, argv + argc};

    NFD_Init();

    try
    {
        auto window = std::make_unique<Simulator::Window>(argv[0]);

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
