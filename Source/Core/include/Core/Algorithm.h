#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>

#include "Maze.h"

namespace Core
{
// Forward-decl
class Mouse;

/*! \brief Base class for algorithms
 *
 *  The Algorithm class is the base class used to implement various algorithms for traversing the
 * maze
 */
class Algorithm
{
public:
    virtual ~Algorithm() = default;

    //! Run a step with the Algorithm and get the direction relative to maze the mouse should move
    virtual std::optional<Direction> Step(Mouse *mouse, int x, int y, Direction direction) = 0;
    //! Get text for a tile, only used in Simulator
    virtual std::optional<std::string> GetText(Mouse *mouse, int x, int y);
};

/*! \brief Registry class for algorithms
 *
 *  The registry enables algorithms to be registered in a static variable
 */
class AlgorithmRegistry
{
public:
    using AlgorithmConstructor = std::function<Algorithm *(Mouse *mouse, int x, int y)>;
    using Registry = std::map<std::string, AlgorithmConstructor>;

    //! Algorithm registration function, used internally by REGISTER_ALGORITHM macro
    //!
    //! \attention Do not call after static initialization, that will screw the algorithm indexes!
    static bool Register(const std::string &name, AlgorithmConstructor constructor);
    //! Access the registry of Algorithm registrated
    static Registry &GetRegistry();
};

} // namespace Core

// Add register macro for algorithms
#define REGISTER_ALGORITHM(ALGORITHM)                                                              \
    bool ALGORITHM##Algorithm = AlgorithmRegistry::Register(                                       \
        #ALGORITHM,                                                                                \
        [](Mouse *mouse, int x, int y) -> Algorithm * { return new ALGORITHM(mouse, x, y); });
