#include "Application.h"
#include "Services/BLE.h"
#include "Services/MainWindow.h"
#include "Services/RemoteMouses.h"
#include "Services/Simulation.h"

namespace Simulator
{

Application::Application(std::vector<std::string> args, Services::BLE *ble)
{
    // Register services
    if (ble != nullptr)
        RegisterService(ble);                                 // BLE
    RegisterService(new Services::MainWindow(this, args[0])); // Main Window
    RegisterService(new Services::RemoteMouses(this));        // Remote mouses
    RegisterService(new Services::Simulation(this));          // Simulation

    // Open txt as maze file as first argument
    if (args.size() > 1)
        GetService<Services::Simulation>()->OpenMaze(args[1]);

    // Register windows
    auto &registry{Windows::WindowRegistry::GetRegistry()};
    for (auto &window_constructor : registry)
    {
        windows[window_constructor.first] = window_constructor.second(this);
    }
}

void Application::Run()
{
    // Continue until quitting
    while (!quit)
    {
        // Tick all the services
        for (const auto &[_, service] : services)
        {
            try
            {
                service->Tick();
            }
            catch (const std::exception &e)
            {
                Error(e.what());
            }
        }

        // Tick all the windows
        for (const auto &[_, window] : windows)
        {
            try
            {
                window->Tick();
            }
            catch (const std::exception &e)
            {
                Error(e.what());
            }
        }
    }
}

void Application::Error(std::string err) { errors.push(err); }

SimulatorMouse *Application::GetSimulatorMouse()
{
    auto remote_mouses{GetService<Services::RemoteMouses>()};
    auto active_remote_mouse{remote_mouses->GetActiveRemoteMouse()};
    if (active_remote_mouse != nullptr)
        return active_remote_mouse;

    // TODO: Remote mouse
    return GetService<Services::Simulation>();
}

Application::~Application()
{
    // Delete windows
    for (const auto &[_, window] : windows)
        delete window;

    // Delete all services
    for (const auto &[_, service] : services)
        delete service;
}

} // namespace Simulator
