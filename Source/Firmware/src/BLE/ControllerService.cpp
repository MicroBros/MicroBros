#include "ControllerService.h"

namespace Firmware::BLE
{

ControllerService::ControllerService(Drivers::DFR0548 &driver) : driver{driver}
{
    // Register the BLE service
    RegisterBaseUUID(bs_base_uuid);
    CreateService(0xAA00);
}

}; // namespace Firmware::BLE
