#include "RemoteMouses.h"

#include "../Application.h"
#include "BLE.h"

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

        SimpleBLE::BluetoothAddress address{peripheral.address()};
        if (!mouses.contains(address))
            mouses[address] = std::make_unique<RemoteMouse>(application, peripheral);
    }

    // Run through all the remote mouses
    for (auto &[_, mouse] : mouses)
    {
        mouse->Tick();
    }
}

/* Remote Mouse impl */

RemoteMouses::RemoteMouse::RemoteMouse(Application *application, SimpleBLE::Peripheral peripheral)
    : application{application}, peripheral{peripheral}
{
}

void RemoteMouses::RemoteMouse::OnConnected() {}

void RemoteMouses::RemoteMouse::OnDisconnected() {}

void RemoteMouses::RemoteMouse::Tick()
{
    bool is_connected{peripheral.is_connected()};
    if (connected != is_connected)
    {
        if (is_connected)
            OnConnected();
        else
            OnDisconnected();

        connected = is_connected;
    }
}

} // namespace Simulator::Services
