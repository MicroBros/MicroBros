#include <Core/Log.h>

#include "MotorService.h"

namespace Firmware::BLE
{

MotorService::MotorService(Drivers::DFR0548 *driver) : driver{driver}
{
    // Register the BLE service
    RegisterBaseUUID(bs_base_uuid);
    CreateService(0x0100);

    // Setup characteristics
    CreateCharacteristic(CHARACTERISTIC(MotorService, Motors), 0x0100 + 1, (uint8_t *)&motors,
                         sizeof(BLE_STRUCTURE(MotorService, Motors)),
                         sizeof(BLE_STRUCTURE(MotorService, Motors)),
                         microbit_propWRITE | microbit_propWRITE_WITHOUT);
}

void MotorService::onDataWritten(const microbit_ble_evt_write_t *params)
{
    if (params->handle == valueHandle(CHARACTERISTIC(MotorService, Motors)))
    {
        BLE_SIZE_CHECK(MotorService, Motors);

        BLE_STRUCTURE(MotorService, Motors) *data =
            (BLE_STRUCTURE(MotorService, Motors) *)params->data;

        // LOG_DEBUG("Setting motor values: lb:{}, lf:{}, rb:{}, rf:{}", data->lb, data->lf,
        // data->rb, data->rf);

        driver->SetMotors(data->lb, data->lf, data->rb, data->rf);
    }
}

}; // namespace Firmware::BLE
