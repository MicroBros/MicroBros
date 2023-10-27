#pragma once

#include <assert.h>
#include <map>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

#include "Services/MainWindow.h"
#include "Services/Service.h"
#include "SimulatorMouse.h"
#include "Windows/Window.h"

namespace Simulator
{

// Forward-decl
namespace Services
{
class BLE;
};

/*! \brief Main class for the simulator instance
 */
class Application
{
public:
    using ServicesMap = std::unordered_map<std::size_t, Services::Service *>;
    using WindowsMap = std::map<Windows::WindowId, Windows::Window *>;

    Application(std::vector<std::string> args, Services::BLE *ble);
    ~Application();

    //! Run the Application event-loop blocking until closed
    void Run();
    //! Push to the error stack and show an error modal with error message
    void Error(std::string err);
    //! Make the Application quit
    inline void Quit() { quit = true; }

    //! Register a service by typeid
    template <typename S> inline void RegisterService(S *service)
    {
        bool has_service_baseclass{std::is_base_of<Services::Service, S>()};
        assert(has_service_baseclass);

        services[typeid(S).hash_code()] = service;
    }

    //! Get a service by typeid
    template <typename S> inline S *GetService()
    {
        if (auto service = services.find(typeid(S).hash_code()); service != services.end())
            return dynamic_cast<S *>((*service).second);
        else
            throw std::runtime_error(
                fmt::format("Unable to find Service of type \"{}\"", typeid(S).name()));
    }

    //! Get service, but will return nullptr if not found
    template <typename S> inline S *GetServiceNullable()
    {
        if (auto service = services.find(typeid(S).hash_code()); service != services.end())
            return dynamic_cast<S *>((*service).second);
        else
            return nullptr;
    }

    template <typename W> inline W *GetWindow()
    {
        if (auto window = windows.find(W::ID); window != windows.end())
            return dynamic_cast<W *>((*window).second);
        else
            throw std::runtime_error(
                fmt::format("Unable to find Window of type \"{}\"", typeid(W).name()));
    }

    inline Windows::Window *GetWindow(Windows::WindowId id)
    {
        if (auto window = windows.find(id); window != windows.end())
            return (*window).second;
        else
            throw std::runtime_error(
                fmt::format("Unable to find Window of id \"{}\"", static_cast<int>(id)));
    }

    //! Get the SimulatorMouse, can be nullptr
    SimulatorMouse *GetSimulatorMouse();

    //! Shortcut to get the MainWindow
    inline Services::MainWindow *GetMainWindow() { return GetService<Services::MainWindow>(); };

private:
    ServicesMap services;
    WindowsMap windows;
    bool quit{false};
    std::queue<std::string> errors;

    // Not ideal, but better than adding getters, MainWindow is in practice an extension of
    // Application with all the platform window code
    friend Services::MainWindow;
};

}; // namespace Simulator
