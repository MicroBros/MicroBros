#pragma once

#include <Core/Mouse.h>

namespace Simulator
{

/*! \brief Base abstract class for controllable Mouse

    Note the SimulatorMouse may not be simulated, but a remote controlled instance!
 */
class SimulatorMouse
{
public:
    //! Returns if the Mouse is being simulated
    virtual bool IsSimulation() = 0;
    //! Returns if the Mouse is currently moving
    virtual bool IsMoving() = 0;
    //! Returns if the Mouse is currently running (program)
    virtual bool IsRunning() = 0;
    //! Returns if the Mouse is currently running (program)
    virtual void SetRunning(bool running) = 0;
    //! Manually step one move
    virtual void Step() = 0;
    //! Reset the Mouse
    virtual void Reset() = 0;
    //! Get the Mouse (Can be nullptr)
    virtual Core::Mouse *GetMouse() = 0;
    /* Algorithms */
    //! Get the list of algorithms
    virtual std::vector<std::string> &GetAlgorithms() = 0;
    //! Get index of current algorithm
    virtual size_t GetAlgorithm() = 0;
    //! Set index of algorithm to use
    virtual void SetAlgorithm(size_t i) = 0;
};

}; // namespace Simulator
