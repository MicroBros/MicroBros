#include <nfd.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "Application.h"
#include "Services/BLE.h"

int main(int argc, char *argv[])
{
    // BLE service needs to be initialised once here, or issues will arrise with
    // WinRT, THANKS MICROSOFT
    Simulator::Services::BLE *ble{nullptr};
    std::optional<std::string> error;

    try
    {
        ble = new Simulator::Services::BLE();
    }
    catch (const std::exception &e)
    {
        error = e.what();
    }

    // Read the args (minus the program) from argv
    std::vector<std::string> args{argv, argv + argc};

    NFD_Init();

    try
    {
        auto application = std::make_unique<Simulator::Application>(args, ble);

        if (error.has_value())
            application->Error(error.value());

        application->Run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }

    NFD_Quit();

    return 0;
}
