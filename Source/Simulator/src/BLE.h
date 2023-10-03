#pragma once

#include <memory>
#include <optional>

#include <simpleble/Adapter.h>

namespace Simulator
{

/*! \brief BLE connection manager

Using SimpleBLE to manage Bluetooth LE connections to the Mouse
 */
class BLE
{
public:
    BLE();
    ~BLE();

    //! Function called by the Window once per every frame to manage different tasks
    void Tick();
    //! Force a scan
    void Scan();

    //! Connect to a BLE device
    void Connect(SimpleBLE::Peripheral &peripheral);
    //! Disconnect a BLE device
    void Disconnect(SimpleBLE::Peripheral &peripheral);
    //! Sets a BLE device as active, will only work if connected
    void SetActive(SimpleBLE::Peripheral &peripheral);
    //! Get the SimpleBLE::Peripheral that is active, if there is one
    std::optional<SimpleBLE::Peripheral> GetActive();
    //! Checks if the BLE peripheral is active
    inline bool IsActive(SimpleBLE::Peripheral &peripheral)
    {
        return peripheral.address() == active_peripheral;
    }

    //! Return alias to BLE autoscan option for continiously scanning
    inline bool &Autoscan() noexcept { return autoscan; }
    //! Return alias to vector of SimpleBLE::Peripheral
    inline std::vector<SimpleBLE::Peripheral> &Peripherals() noexcept { return peripherals; }
    //! Set if the BLE window is currently open which impacts behaivour like autoscan
    inline void SetWindowOpen(bool open) noexcept { window_open = open; }

private:
    //! Checks the vendor/other info for if the device should even be connectable
    bool ConnectablePeripheral(SimpleBLE::Peripheral &peripheral);

    bool autoscan{true};
    bool window_open{false};
    uint64_t last_scan{0};
    std::string active_peripheral;
    std::unique_ptr<SimpleBLE::Adapter> adapter{nullptr};
    std::vector<SimpleBLE::Peripheral> peripherals;
};

}; // namespace Simulator
