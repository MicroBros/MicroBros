#pragma once

#include <functional>
#include <map>
#include <memory>

#include <Core/Comm.h>
#include <Core/Mouse.h>
#include <simpleble/Adapter.h>

#include "../SimulatorMouse.h"
#include "BLE.h"
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
    class RemoteMouse : public SimulatorMouse
    {
    public:
        RemoteMouse(Application *application, BLE *ble, SimpleBLE::BluetoothAddress address);

        void OnConnected();
        void OnDisconnected();

        void Tick();

        /* SimulatorMouse methods */
        inline bool IsSimulation() noexcept { return false; };
        inline bool IsMoving() noexcept { return moving; };
        inline bool IsRunning() noexcept { return control.running; };
        void SetRunning(bool running);
        void SetReturning(bool returning);
        void Reset();
        void Step();
        inline Core::Mouse *GetMouse() noexcept { return mouse.get(); };
        std::vector<std::string> &GetAlgorithms();
        inline size_t GetAlgorithm() noexcept { return control.current_algorithm; }
        void SetAlgorithm(size_t i);
        inline Core::Maze *GetMaze() noexcept { return mouse->GetMaze(); };

    private:
        void OnControlNotify(SimpleBLE::ByteArray data);
        void OnPositionNotify(SimpleBLE::ByteArray data);
        void OnMazeNotify(SimpleBLE::ByteArray data);

        void SendAction(BLE_STRUCTURE(MouseService, MouseAction) action);
        void UpdateControl();

        bool connected{false};
        bool moving{false};
        Application *application;
        BLE *ble;
        SimpleBLE::BluetoothAddress address;
        SimpleBLE::Peripheral peripheral;

        // Data
        BLE_STRUCTURE(MouseService, AlgorithmCount) algorithm_count;
        BLE_STRUCTURE(MouseService, MouseControl) control;
        std::unique_ptr<Core::Mouse> mouse;
        std::vector<std::string> algorithms;
    };

    RemoteMouse *GetActiveRemoteMouse();

private:
    Application *application;
    std::unordered_map<SimpleBLE::BluetoothAddress, std::unique_ptr<RemoteMouse>> mouses;
};

}; // namespace Simulator::Services
