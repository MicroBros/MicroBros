#pragma once

#include <fmt/format.h>
#include <stdint.h>
#include <stdlib.h>

//! Macro to create the MicroBit BLE UUID for SimpleBLE
#define MICROBIT_BLE_UUID(uuid) fmt::format("e95d{:04x}-251d-470a-a062-fa1922dfa9a8", uuid)
//! Get the namespaced ident for BLE structure for service
#define BLE_STRUCTURE(service, struct) Core::Comm::service::struct
//! Get the index for the Characteristic
#define CHARACTERISTIC(service, name)                                                              \
    static_cast<uint16_t>(Core::Comm::service::Characteristics::name)
#define CHARACTERISTIC_UUID(service, name)                                                         \
    Core::Comm::service::CharacteristicUuid(Core::Comm::service::Characteristics::name)
//! Get the Characteristic count
#define CHARACTERISTICS_COUNT(service) static_cast<int>(Core::Comm::service::Characteristics::Count)
//! BLE structure size check for CODAL
#define BLE_SIZE_CHECK(service, struct)                                                            \
    if (params->len != sizeof(BLE_STRUCTURE(service, struct)))                                     \
    {                                                                                              \
        LOG_ERROR("Incorrect size of {}, got: {}, expected: {}", #struct, params->len,             \
                  sizeof(BLE_STRUCTURE(service, struct)));                                         \
        return;                                                                                    \
    }
//! Get the MicroBit UUID String for service
#define MICROBIT_BLE_SERVICE_UUID(service) MICROBIT_BLE_UUID(Core::Comm::service::UUID)
//! Get the MicroBit UUID String for service characteristic
#define MICROBIT_BLE_CHARACTERISTIC_UUID(service, characteristic)                                  \
    MICROBIT_BLE_UUID(CHARACTERISTIC_UUID(service, characteristic))

namespace Core::Comm
{

//! BLE structures and IDs related to Firmware::BLE::MotorService
namespace MotorService
{

const uint16_t UUID = 0x0100;

//! BLE Characteristics for MotorService
enum class Characteristics : uint8_t
{
    Motors = 0,
    Count = 1,
};

inline int16_t CharacteristicUuid(Characteristics c)
{
    switch (c)
    {
    case Characteristics::Motors:
        return UUID + 1;
    default:
        abort();
    }
}

//! Struct for Motor values
struct Motors
{
    //! Right front
    int16_t rf;
    //! Right back
    int16_t rb;
    //! Left front
    int16_t lf;
    //! Left back
    int16_t lb;
};

}; // namespace MotorService

}; // namespace Core::Comm
