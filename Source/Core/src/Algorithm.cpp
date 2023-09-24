#include "Core/Algorithm.h"

namespace Core
{

std::optional<std::string> Algorithm::GetText(Maze *maze, int x, int y) { return std::nullopt; }

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
