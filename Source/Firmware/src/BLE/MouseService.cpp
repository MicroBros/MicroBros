#include <algorithm>
#include <cmath>

#include <Core/Algorithm.h>
#include <Core/Log.h>

#include "MouseService.h"

namespace Firmware::BLE
{

// Dummy char, used for empty charactistics
static uint8_t dummy;

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
                             microbit_propREADAUTH | microbit_propNOTIFY);
    // Action
    CreateCharacteristic(CHARACTERISTIC(MouseService, Action),
                         CHARACTERISTIC_UUID(MouseService, Action), &dummy, 1, 1,
                         microbit_propWRITE | microbit_propWRITE_WITHOUT);

    // GetAlgorithmCount
    static BLE_STRUCTURE(MouseService, AlgorithmCount) algorithm_count =
        Core::AlgorithmRegistry::GetRegistry().size();
    CreateCharacteristic(CHARACTERISTIC(MouseService, GetAlgorithmCount),
                         CHARACTERISTIC_UUID(MouseService, GetAlgorithmCount),
                         (uint8_t *)&algorithm_count, sizeof(algorithm_count),
                         sizeof(algorithm_count), microbit_propREAD | microbit_propREADAUTH);
    // GetAlgorithmName
    CreateCharacteristic(CHARACTERISTIC(MouseService, GetAlgorithmName),
                         CHARACTERISTIC_UUID(MouseService, GetAlgorithmName),
                         (uint8_t *)algorithm_name_buffer.data(),
                         algorithm_name_buffer.size() * sizeof(algorithm_name_buffer[0]),
                         algorithm_name_buffer.size() * sizeof(algorithm_name_buffer[0]),
                         microbit_propREAD | microbit_propREADAUTH | microbit_propWRITE |
                             microbit_propWRITE_WITHOUT);
    // Position
    CreateCharacteristic(CHARACTERISTIC(MouseService, Position),
                         CHARACTERISTIC_UUID(MouseService, Position), (uint8_t *)&position,
                         sizeof(position), sizeof(position),
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
    // Maze
    CreateCharacteristic(CHARACTERISTIC(MouseService, Maze),
                         CHARACTERISTIC_UUID(MouseService, Maze), tile_values.data(),
                         tile_values.size() * sizeof(tile_values[0]),
                         tile_values.size() * sizeof(tile_values[0]),
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
}

void MouseService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Control)))
    {
        BLE_SIZE_CHECK(MouseService, MouseControl);

        BLE_STRUCTURE(MouseService, MouseControl) *
            control_ptr{(BLE_STRUCTURE(MouseService, MouseControl) *)params->data};
        control = *control_ptr;
        // Update mouse
        if (mouse->IsRunning() != control.running)
            mouse->SetRunning(control.running);
        mouse->ReturnStart() = control.returning;
        mouse->SetResetAlgorithm(control.algorithm);
    }
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, GetAlgorithmName)))
    {
        BLE_SIZE_CHECK(MouseService, AlgorithmCount);

        algorithm_index = *(BLE_STRUCTURE(MouseService, AlgorithmCount) *)params->data;

        auto registry{Core::AlgorithmRegistry::GetRegistry()};

        auto it = registry.begin();
        for (size_t i{0}; i < algorithm_index; ++i)
        {
            it++;

            // Return if OOB
            if (it == registry.end())
            {
                algorithm_name_buffer[0] = '\0';
                return;
            }
        }

        // Copy algorithm name
        std::copy_n(it->first.begin(), std::min(it->first.size(), size_t(MAX_ALGORITHM_NAME)),
                    algorithm_name_buffer.begin());

        algorithm_name_buffer[std::min(it->first.size(), size_t(MAX_ALGORITHM_NAME))] = '\0';
    }
    else if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Action)))
    {
        BLE_SIZE_CHECK(MouseService, MouseAction);

        auto action{*(BLE_STRUCTURE(MouseService, MouseAction) *)params->data};

        switch (action)
        {
        // Reset
        case BLE_STRUCTURE(MouseService, MouseAction)::Reset:
            mouse->SetRunning(false);
            mouse->Reset();

            break;
        // Step
        case BLE_STRUCTURE(MouseService, MouseAction)::Step:
            mouse->SetRunning(false);

            // Wait until not running (finished movement)
            while (mouse->IsMoving())
            {
                fiber_sleep(50);
            }

            LOG_INFO("[BLE] Step");

            mouse->Step();

            break;
        default:
            break;
        }
    }
}

void MouseService::onDataRead(microbit_onDataRead_t *params)
{
    if (params->handle == valueHandle(CHARACTERISTIC(MouseService, Maze)))
    {
        UpdateTiles();
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
    position.moving = mouse->IsMoving();
    position.x = mouse->X() * INT_FLOAT_DIV;
    position.y = mouse->Y() * INT_FLOAT_DIV;
    position.rot = mouse->Rot() * INT_FLOAT_DIV;
    notifyChrValue(CHARACTERISTIC(MouseService, Position), (const uint8_t *)&position,
                   sizeof(position));
}

void MouseService::UpdateTiles()
{
    auto tiles{mouse->GetMaze()->Data()};

    // Update size if needed
    if (tiles.size() != tile_values.size())
    {
        LOG_ERROR("[BLE] Only 16x16 mazes are supported");
        return;
    }

    // Update copy values
    for (size_t i{0}; i < tiles.size(); ++i)
    {
        tile_values[i] = tiles[i].Value();
    }
}

void MouseService::MazeUpdate()
{
    // Ignore if disconnected
    if (!getConnected())
        return;

    UpdateTiles();

    notifyChrValue(CHARACTERISTIC(MouseService, Maze), (const uint8_t *)tile_values.data(),
                   sizeof(Core::MazeTile::ValueType) * tile_values.size());
}

}; // namespace Firmware::BLE
