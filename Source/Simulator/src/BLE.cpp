#include <SDL.h>
#include <fmt/format.h>

#include <Core/Log.h>

#include "BLE.h"

// Start a new scan every second
const uint64_t AUTO_SCAN_INTERVAL{1000};

// Update the

namespace Simulator
{

BLE::BLE()
{
    std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
    if (adapters.empty())
        throw std::runtime_error("No BLE adapters found!");

    // Pick the first adapter
    adapter = std::make_unique<SimpleBLE::Adapter>(std::move(adapters[0]));
    // Start scanning
    adapter->scan_start();
}

BLE::~BLE() { adapter->scan_stop(); }

void BLE::Tick()
{
    if (!adapter)
        return;

    if (autoscan && SDL_GetTicks() - last_scan > AUTO_SCAN_INTERVAL)
    {
        Scan();
    }
}

void BLE::Scan()
{
    if (!adapter)
        return;

    if (!adapter->bluetooth_enabled())
    {
        autoscan = false;
        throw std::runtime_error("Bluetooth is not enabled");
    }

    peripherals.clear();

    for (auto p : adapter->scan_get_results())
    {
        if (ConnectablePeripheral(p))
            peripherals.push_back(p);
    }
}

void BLE::Connect(SimpleBLE::Peripheral &peripheral)
{
    LOG_INFO("Connecting to BLE device Addr:{}, Ident:{}", peripheral.address(),
             peripheral.identifier());
    peripheral.connect();
    active_peripheral = peripheral.address();
    LOG_INFO("Connected");
}

void BLE::Disconnect(SimpleBLE::Peripheral &peripheral)
{
    LOG_INFO("Disconnecting to BLE device Addr:{}, Ident:{}", peripheral.address(),
             peripheral.identifier());
    peripheral.disconnect();
}

void BLE::SetActive(SimpleBLE::Peripheral &peripheral)
{
    if (peripheral.is_connected())
        active_peripheral = peripheral.identifier();
}

std::optional<SimpleBLE::Peripheral> BLE::GetActive()
{
    for (auto &peripheral : peripherals)
    {
        if (peripheral.identifier() == active_peripheral && peripheral.is_connected())
            return peripheral;
    }

    return std::nullopt;
}

bool BLE::ConnectablePeripheral(SimpleBLE::Peripheral &peripheral)
{
    // Only allow micro:bit BLEs for now
    return peripheral.identifier().rfind("BBC micro:bit", 0) == 0;
}

} // namespace Simulator
