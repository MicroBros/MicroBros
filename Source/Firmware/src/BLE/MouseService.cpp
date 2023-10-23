#include <Core/Algorithm.h>
#include <Core/Log.h>

#include "MouseService.h"

namespace Firmware::BLE
{

MouseService::MouseService(Mouse2 *mouse) : mouse{mouse}
{
    // Register the BLE service
    RegisterBaseUUID(bs_base_uuid);
    CreateService(BLE_SERVICE_UUID(MouseService));

    // Setup characteristics
    // Control
    CreateCharacteristic(CHARACTERISTIC(MouseService, Control),
                         CHARACTERISTIC_UUID(MouseService, Control), (uint8_t *)&control,
                         sizeof(BLE_STRUCTURE(MouseService, MouseControl)),
                         sizeof(BLE_STRUCTURE(MouseService, MouseControl)),
                         microbit_propWRITE | microbit_propWRITE_WITHOUT | microbit_propREAD |
                             microbit_propREADAUTH);
    // Step
    CreateCharacteristic(CHARACTERISTIC(MouseService, Step),
                         CHARACTERISTIC_UUID(MouseService, Step), nullptr, 0, 0,
                         microbit_propWRITE | microbit_propWRITE_WITHOUT);

    // Reset
    CreateCharacteristic(CHARACTERISTIC(MouseService, Reset),
                         CHARACTERISTIC_UUID(MouseService, Reset), nullptr, 0, 0,
                         microbit_propWRITE | microbit_propWRITE_WITHOUT);

    // GetAlgorithmCount
    CreateCharacteristic(CHARACTERISTIC(MouseService, GetAlgorithmCount),
                         CHARACTERISTIC_UUID(MouseService, GetAlgorithmCount), nullptr, 0, 0,
                         microbit_propREAD | microbit_propREADAUTH);
    // GetAlgorithmName
    CreateCharacteristic(CHARACTERISTIC(MouseService, GetAlgorithmName),
                         CHARACTERISTIC_UUID(MouseService, GetAlgorithmName), nullptr, 0, 0,
                         microbit_propREAD | microbit_propREADAUTH);
    // Position
    CreateCharacteristic(CHARACTERISTIC(MouseService, Position),
                         CHARACTERISTIC_UUID(MouseService, Position), (uint8_t *)&position,
                         sizeof(BLE_STRUCTURE(MouseService, MousePosition)),
                         sizeof(BLE_STRUCTURE(MouseService, MousePosition)),
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
    // Maze
    CreateCharacteristic(CHARACTERISTIC(MouseService, Maze),
                         CHARACTERISTIC_UUID(MouseService, Maze), nullptr, 0, 0,
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
}

void MouseService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Control)))
    {
        BLE_SIZE_CHECK(MouseService, MouseControl);

        control = *(BLE_STRUCTURE(MouseService, MouseControl) *)params->data;
        // Update mouse
        if (mouse->IsRunning() != control.running)
            mouse->SetRunning(control.running);
        mouse->ReturnStart() = control.returning;
        mouse->SetResetAlgorithm(control.algorithm);
    }
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Step)))
    {
        mouse->SetRunning(false);

        // Wait until not running (finished movement)
        while (mouse->IsMoving())
        {
            fiber_sleep(50);
        }

        mouse->Step();
    }
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Reset)))
    {
        mouse->SetRunning(false);
        mouse->Reset();
    }
}

void MouseService::onDataRead(microbit_onDataRead_t *params)
{
    // Return amount of algorithms
    if (params->handle == valueHandle(CHARACTERISTIC(MouseService, GetAlgorithmCount)))
    {
        static int16_t algorithm_count;
        algorithm_count = Core::AlgorithmRegistry::GetRegistry().size();
        params->data = (const uint8_t *)&algorithm_count;
        params->length = sizeof(algorithm_count);
    }
    // Get algorithm at offset
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, GetAlgorithmName)))
    {
        auto registry{Core::AlgorithmRegistry::GetRegistry()};

        auto it = registry.begin();
        for (size_t i{0}; i < params->offset; ++i)
        {
            it++;

            // Return if OOB
            if (it == registry.end())
            {
                params->length = 0;
                return;
            }
        }

        // Return name in string
        params->data = (const uint8_t *)it->first.c_str();
        params->length = it->first.size();
    }
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Maze)))
    {
        auto tiles{mouse->GetMaze()->Data()};
        params->data = (const uint8_t *)tiles.data();
        params->length = sizeof(tiles[0]) * tiles.size();
    }
}

void MouseService::Update()
{
    // Ignore if disconnected
    if (!getConnected())
        return;

    // Check if running has been toggled
    if (control.running != mouse->IsRunning() ||
        control.current_algorithm != mouse->GetAlgorithmIndex())
    {
        control.running = mouse->IsRunning();
        control.current_algorithm = mouse->GetAlgorithmIndex();
        notifyChrValue(CHARACTERISTIC(MouseService, Control), (const uint8_t *)&control,
                       sizeof(control));
    }

    // Update position of mouse
    position.x = mouse->X();
    position.y = mouse->Y();
    position.rot = mouse->Rot();
    notifyChrValue(CHARACTERISTIC(MouseService, Position), (const uint8_t *)&position,
                   sizeof(position));
}

void MouseService::MazeUpdate()
{
    // Ignore if disconnected
    if (!getConnected())
        return;

    auto tiles{mouse->GetMaze()->Data()};
    notifyChrValue(CHARACTERISTIC(MouseService, Maze), (const uint8_t *)tiles.data(),
                   sizeof(tiles[0]) * tiles.size());
}

}; // namespace Firmware::BLE
