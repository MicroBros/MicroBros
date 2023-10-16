#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

// Forward-decl
namespace Simulator
{
class Application;
}; // namespace Simulator

namespace Simulator::Windows
{

// Add a window id per window
enum class WindowId
{
    Maze,
    Controls,
    RemoteConnections,
    RemoteMotors
};

//! Base abstract class for simulator windows
class Window
{
public:
    //! Called inside of ImGui UI state
    virtual void Draw() = 0;
    //! Non UI-related work
    virtual void Tick();
    //! Get if the window is open
    virtual bool IsOpen();
    //! Get alias to bool for window is open
    virtual bool &GetOpen();
    //! Set if the window is open
    virtual void SetOpen(bool open);

private:
    bool open{false};
};

/*! \brief Registry class for Window
 *
 *  The registry enables Window to be registered in a static variable
 */
class WindowRegistry
{
public:
    using WindowConstructor = std::function<Window *(Application *application)>;
    using Registry = std::unordered_map<WindowId, WindowConstructor>;

    //! Window registration function, used internally by REGISTER_Window macro
    static bool Register(WindowId id, WindowConstructor constructor);
    //! Access the registry of Window registrated
    static Registry &GetRegistry();
};

} // namespace Simulator::Windows

// Add register macro for Window
#define REGISTER_WINDOW(WINDOW)                                                                    \
    bool Window##WINDOW = WindowRegistry::Register(                                                \
        WINDOW::ID, [](Application *application) -> Window * { return new WINDOW(application); });

#define WINDOW(id) static const WindowId ID{WindowId::id};
