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
                         CHARACTERISTIC_UUID(MouseService, Position), nullptr, 0, 0,
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
    // Maze
    CreateCharacteristic(CHARACTERISTIC(MouseService, Maze),
                         CHARACTERISTIC_UUID(MouseService, Maze), nullptr, 0, 0,
                         microbit_propREAD | microbit_propREADAUTH | microbit_propNOTIFY);
}

void MouseService::onDataWritten(const microbit_ble_evt_write_t *params) {}

}; // namespace Firmware::BLE
