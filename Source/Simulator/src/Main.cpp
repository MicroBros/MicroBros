#include <nfd.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "Window.h"

#include "BLE.h"

int main(int argc, char *argv[])
{
    std::unique_ptr<Simulator::BLE> ble{nullptr};
    std::optional<std::string> error;
    // BLE needs to be initialised once here, or issues will arrise with
    // WinRT, THANKS MICROSOFT
    try
    {
        ble.reset(new Simulator::BLE());
    }
    catch (const std::exception &e)
    {
        error = e.what();
    }

    // Read the args (minus the program) from argv
    std::vector<std::string> args{argv + 1, argv + argc};

    NFD_Init();

    try
    {
        auto window = std::make_unique<Simulator::Window>(argv[0], std::move(ble));

        // Parse command arguments
        if (!args.empty())
        {
            // Check if the first argument is a .txt
            if (std::filesystem::path(args[0]).extension() == ".txt")
            {
                window->OpenMaze(args[0]);
            }
        }
        if (error.has_value())
            window->Error(error.value());

        window->Run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }

    NFD_Quit();

    return 0;
}
