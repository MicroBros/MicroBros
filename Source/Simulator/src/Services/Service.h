#pragma once

// Forward-decl
namespace Simulator
{
class Application;
};

namespace Simulator::Services
{

/*! \brief Base class for services
 */
class Service
{
public:
    // Setup virtual deconstructor
    virtual ~Service() = default;

    //! Main tick function of Service, ran in the main Application loop
    virtual void Tick() = 0;
};

} // namespace Simulator::Services
