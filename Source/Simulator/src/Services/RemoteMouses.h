#pragma once

#include <map>
#include <memory>

#include <simpleble/Adapter.h>

#include "Service.h"

namespace Simulator::Services
{

/*! \brief Remote mouse service (BLE)

Using RemoteMouses to manage remote SimulatorMouse over BLE through RemoteMouse
*/
class RemoteMouses : public Service
{
public:
    RemoteMouses(Application *application);

    void Tick();

    //! Single instance of a remote mouse
    class RemoteMouse
    {
    public:
        RemoteMouse(Application *application, SimpleBLE::Peripheral peripheral);

        void OnConnected();
        void OnDisconnected();

        void Tick();

    private:
        bool connected{false};
        Application *application;
        SimpleBLE::Peripheral peripheral;
    };

private:
    Application *application;
    std::unordered_map<SimpleBLE::BluetoothAddress, std::unique_ptr<RemoteMouse>> mouses;
};

}; // namespace Simulator::Services
