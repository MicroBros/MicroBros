#include "Window.h"

namespace Simulator::Windows
{

// Default as no-op
void Window::Tick() {}

bool Window::IsOpen() { return GetOpen(); }

bool &Window::GetOpen() { return open; }

void Window::SetOpen(bool open) { this->open = open; }

bool WindowRegistry::Register(WindowId id, WindowConstructor constructor)
{
    auto &registry{GetRegistry()};

    registry[id] = constructor;

    return true;
}

//! Access the registry of Window registrated
WindowRegistry::Registry &WindowRegistry::GetRegistry()
{
    static Registry registry;

    return registry;
}

} // namespace Simulator::Windows
