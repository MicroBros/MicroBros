#include "Mouse.h"

namespace Firmware
{

Mouse::Mouse(MicroBit &uBit, Firmware::Drivers::DFR0548 &driver) : uBit{uBit}, driver{driver} {}

void Mouse::Run()
{
    UpdateAcceleration();
    UpdateVelocity();
    UpdatePosition();
    Debug();
    ++iteration;
}

void Mouse::Debug()
{

    this->uBit.serial.printf("Iter: ", iteration, "\n");

    this->uBit.serial.printf("Time_us", uBit.timer.getTimeUs(), "\n");

    this->uBit.serial.printf("Delta time us:", delta_time_us, "\n");

    this->uBit.serial.printf("Acceleration x: ", acceleration_x, "\n");
    this->uBit.serial.printf("Acceleration y: ", acceleration_y, "\n");

    this->uBit.serial.printf("Velocity x: ", velocity_x, "\n");
    this->uBit.serial.printf("Velocity y: ", velocity_y, "\n");

    this->uBit.serial.printf("Position x: ", position_x, "\n");
    this->uBit.serial.printf("Position y: ", position_y, "\n");

    this->uBit.serial.printf("----------------------", "\n");
}

// UpdateVector() assumes the compass heading is correct
void Mouse::UpdateAcceleration()
{
    uint64_t start_time = this->uBit.timer.getTimeUs();

    float acc_x = this->uBit.accelerometer.getX();
    float acc_y = this->uBit.accelerometer.getZ();
    float hdg_rad = NormaliseRad(Deg2Rad(this->uBit.compass.heading()));

    this->acceleration_x = (acc_x * std::cos(hdg_rad) + acc_y * std::sin(hdg_rad));
    this->acceleration_y = acc_y * std::cos(hdg_rad) - acc_x * std::sin(hdg_rad);

    this->delta_time_us = this->uBit.timer.getTimeUs() - start_time;
    this->delta_time_s = this->delta_time_us / 1'000'000;
};

//! Calculates velocity and converts from millimeters per second squared to meters per second
void Mouse::UpdateVelocity()
{
    this->velocity_x += (this->acceleration_x / 1'000) * this->delta_time_s; // meters per second
    this->velocity_y += (this->acceleration_y / 1'000) * this->delta_time_s; // meters per second
}

//! Calculates position and converts to meters.
void Mouse::UpdatePosition()
{
    this->position_x += (this->velocity_x * this->delta_time_s) * 0.5 *
                        ((this->acceleration_x / 1'000) * std::pow(this->delta_time_s, 2));
    this->position_y += (this->velocity_y * this->delta_time_s) * 0.5 *
                        ((this->acceleration_y / 1'000) * std::pow(this->delta_time_s, 2));
}

void Mouse::UpdateHeading()
{
    // Shall return correct heading
    // this->rot = m_uBit.compass.heading();
}

} // namespace Firmware