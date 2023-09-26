#pragma once
#include <MicroBit.h>

#include <Core/Mouse.h>

#include <math.h>

#include "Drivers/DFR0548.h"

#include "Utils.h"

namespace Firmware
{

class Mouse : public Core::Mouse
{

public:
    Mouse(MicroBit &uBit, Firmware::Drivers::DFR0548 &driver);

private:
    MicroBit &uBit;
    Firmware::Drivers::DFR0548 &driver;

    float dx = 0;
    float dy = 0;

    //! Zero = North, Positive direction = Clockwise
    void UpdateHeading();

    void UpdateVector();
};

} // namespace Firmware
