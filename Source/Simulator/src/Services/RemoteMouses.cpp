#include <Core/Log.h>

#include "RemoteMouses.h"

#include "../Application.h"

namespace Simulator::Services
{

RemoteMouses::RemoteMouses(Application *application) : application{application} {}

void RemoteMouses::Tick()
{
    // Get BLE service
    auto ble{application->GetServiceNullable<BLE>()};
    if (!ble)
        return;

    // Ensure that every mouse is added
    for (auto &peripheral : ble->Peripherals())
    {
        // Don't bother if not connected
        if (!peripheral.is_connected())
            continue;

        // Skip if already registered
        SimpleBLE::BluetoothAddress address{peripheral.address()};
        if (mouses.contains(address))
            continue;

        // Still not ready
        if (peripheral.services().empty())
            continue;

        mouses[address] = std::make_unique<RemoteMouse>(application, ble, address);
    }

    // Run through all the remote mouses
    for (auto &[_, mouse] : mouses)
    {
        mouse->Tick();
    }
}

RemoteMouses::RemoteMouse *RemoteMouses::GetActiveRemoteMouse()
{
    auto ble{application->GetServiceNullable<BLE>()};
    if (!ble)
        return nullptr;

    for (auto &peripheral : ble->Peripherals())
    {
        // Don't bother if not connected
        if (ble->IsActive(peripheral) && peripheral.is_connected())
            return mouses[peripheral.address()].get();
    }

    return nullptr;
}

/* Remote Mouse impl */

RemoteMouses::RemoteMouse::RemoteMouse(Application *application, BLE *ble,
                                       SimpleBLE::BluetoothAddress address)
    : application{application}, ble{ble}, address{address}
{
    mouse.reset(new Core::Mouse());
}

// Ugly shortcut
#define RemoteMouse RemoteMouses::RemoteMouse

void RemoteMouse::OnConnected()
{
    LOG_DEBUG("[Remote] Connected {} ({})", peripheral.identifier(), peripheral.address());

    // Get algorithm count
    auto algorithm_count_payload{
        peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, GetAlgorithmCount))};
    BLE_SIZE_CHECK(MouseService, AlgorithmCount, algorithm_count_payload.size());
    algorithm_count =
        *(BLE_STRUCTURE(MouseService, AlgorithmCount) *)algorithm_count_payload.data();

    // Read algorithm names
    algorithms.clear();
    for (BLE_STRUCTURE(MouseService, AlgorithmCount) i{0}; i < algorithm_count; ++i)
    {
        // Set index to read
        peripheral.write_command(
            MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, GetAlgorithmName),
            std::string((char *)&i, sizeof(i)));
        // Read algorithm name
        auto name{
            peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, GetAlgorithmName))};
        algorithms.push_back(name);
    }

    // Subscribe to control updates
    peripheral.notify(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Control),
                      std::bind(&RemoteMouse::OnControlNotify, this, std::placeholders::_1));

    // Manually read the control
    OnControlNotify(peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Control)));

    // Subscribe to position updates
    peripheral.notify(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Position),
                      std::bind(&RemoteMouse::OnPositionNotify, this, std::placeholders::_1));

    // Manually read the position
    OnPositionNotify(peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Position)));

    // Subscribe to maze updates
    peripheral.notify(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Maze),
                      std::bind(&RemoteMouse::OnMazeNotify, this, std::placeholders::_1));
    // Manually read the maze
    OnMazUpdated(peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Maze)));
}

void RemoteMouse::OnDisconnected()
{
    LOG_DEBUG("[Remote] Disconnected {} ({})", peripheral.identifier(), peripheral.address());
}

void RemoteMouse::OnControlNotify(SimpleBLE::ByteArray payload)
{
    // Check if it is actually a MousePosition in size
    BLE_SIZE_CHECK(MouseService, MouseControl, payload.size());

    auto control_upd = (BLE_STRUCTURE(MouseService, MouseControl) *)payload.data();
    control = *control_upd;
}

void RemoteMouse::OnPositionNotify(SimpleBLE::ByteArray payload)
{
    // Check if it is actually a MousePosition in size
    BLE_SIZE_CHECK(MouseService, MousePosition, payload.size());

    auto position = (BLE_STRUCTURE(MouseService, MousePosition) *)payload.data();

    moving = position->moving;

    mouse->SetPosition(position->x / INT_FLOAT_DIV, position->y / INT_FLOAT_DIV,
                       position->rot / INT_FLOAT_DIV);
}

void RemoteMouse::OnMazeNotify(SimpleBLE::ByteArray payload)
{
    // Manually read the maze as notify cannot handle the entire payload :(
    OnMazUpdated(peripheral.read(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Maze)));
}

void RemoteMouse::OnMazUpdated(SimpleBLE::ByteArray data)
{
    // 1 tile is 1 byte
    size_t size{data.size() / sizeof(Core::MazeTile::ValueType)};
    size_t side{size_t(std::sqrt(size))};
    // Verify the maze is square
    if ((side * side) != size)
    {
        LOG_ERROR("Maze is not square, got side:{} and size:{}", side, data.size());
    }

    // Update maze
    std::span<Core::MazeTile::ValueType> tiles{(Core::MazeTile::ValueType *)data.data(), size};
    mouse->SetMaze(new Core::Maze(side, side, tiles));
}

void RemoteMouse::SetRunning(bool running)
{
    control.running = running;
    UpdateControl();
}

void RemoteMouse::SetReturning(bool returning)
{
    control.returning = returning;
    UpdateControl();
}

void RemoteMouse::SendAction(BLE_STRUCTURE(MouseService, MouseAction) action)
{
    peripheral.write_command(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Action),
                             std::string{(char *)&action, sizeof(action)});
}

void RemoteMouse::UpdateControl()
{
    peripheral.write_command(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Control),
                             std::string((char *)&control, sizeof(control)));
}

void RemoteMouse::Reset() { SendAction(BLE_STRUCTURE(MouseService, MouseAction)::Reset); }
void RemoteMouse::Step() { SendAction(BLE_STRUCTURE(MouseService, MouseAction)::Step); }

std::vector<std::string> &RemoteMouse::GetAlgorithms() { return algorithms; }

void RemoteMouse::SetAlgorithm(size_t i)
{
    control.algorithm = i;
    peripheral.write_command(MICROBIT_BLE_SERVICE_CHARACTERISTIC(MouseService, Control),
                             std::string((char *)&control, sizeof(control)));
}

void RemoteMouse::Tick()
{
    // Get updated peripheral from BLE service
    auto new_peripheral{ble->GetByAddress(address)};
    if (!new_peripheral.has_value())
    {
        if (connected)
            OnDisconnected();
        connected = false;

        return;
    }
    peripheral = new_peripheral.value();

    // Check if updated
    bool is_connected{peripheral.is_connected()};
    if (connected != is_connected)
    {

        connected = is_connected;

        if (is_connected)
            OnConnected();
        else
            OnDisconnected();
    }
    // Update state
    else if (connected)
    {
        mouse->ReturnStart() = control.returning;
    }
}

} // namespace Simulator::Services
