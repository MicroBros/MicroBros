#include "Application.h"
#include "Services/BLE.h"
#include "Services/MainWindow.h"
#include "Services/Simulation.h"

namespace Simulator
{

Application::Application(std::vector<std::string> args, Services::BLE *ble)
{
    // Register services
    RegisterService(ble);                                     // BLE
    RegisterService(new Services::MainWindow(this, args[0])); // Main Window
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
            service->Tick();

        // Tick all the windows
        for (const auto &[_, window] : windows)
            window->Tick();
    }
}

void Application::Error(std::string err) { errors.push(err); }

SimulatorMouse *Application::GetSimulatorMouse()
{
    // TODO: Remote mouse
    return GetService<Services::Simulation>();
}

Core::Mouse *Application::GetMouse()
{
    auto sim_mouse{GetSimulatorMouse()};
    if (!sim_mouse)
        return nullptr;
    return sim_mouse->GetMouse();
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
