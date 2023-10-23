#pragma once

#include <fmt/format.h>
#include <stdint.h>
#include <stdlib.h>

//! Macro to create the MicroBit BLE UUID for SimpleBLE
#define MICROBIT_BLE_UUID(uuid) fmt::format("e95d{:04x}-251d-470a-a062-fa1922dfa9a8", uuid)
//! Get the namespaced ident for BLE structure for service
#define BLE_STRUCTURE(service, struct) Core::Comm::service::struct
//! Get the UUID for a service
#define BLE_SERVICE_UUID(service) Core::Comm::service::UUID
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

//! Generate the charactisitics offsets
#define IMPL_CHARACTERISTIC(C)                                                                     \
    inline int16_t CharacteristicUuid(C c) { return UUID + static_cast<int16_t>(c) + 1; }

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
IMPL_CHARACTERISTIC(Characteristics)

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

//! BLE structures and IDs related to Firmware::BLE::MouseService
namespace MouseService
{

const uint16_t UUID = 0x0110;

//! BLE Characteristics for MouseService
enum class Characteristics : uint8_t
{
    Control = 0,
    Step,
    GetAlgorithmCount,
    GetAlgorithm,
    Position,
    Maze,
    Count,
};
IMPL_CHARACTERISTIC(Characteristics)

//! Struct holding information about updating mouse parameters
struct MouseControl
{
    bool running;
    bool returning;
    uint16_t algorithm;
    float speed_factor;
};

//! Updated data from mouse containing position and rotation
struct MousePosition
{
    float x;
    float y;
    float rot;
};

}; // namespace MouseService

}; // namespace Core::Comm
