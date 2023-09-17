#include "Core/Algorithm.h"

#include <iostream>

namespace Core
{

bool AlgorithmRegistry::Register(const std::string &name, AlgorithmConstructor constructor)
{
    auto &registry{GetRegistry()};

    registry[name] = constructor;

    return true;
}

//! Access the registry of Algorithm registrated
AlgorithmRegistry::Registry &AlgorithmRegistry::GetRegistry()
{
    static Registry registry;

    return registry;
}

} // namespace Core
