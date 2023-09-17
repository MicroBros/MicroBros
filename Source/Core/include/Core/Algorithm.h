#pragma once

#include <functional>
#include <map>
#include <string>

namespace Core
{

/*! \brief Base class for algorithms
 *
 *  The Algorithm class is the base class used to implement various algorithms for traversing the
 * maze
 */
class Algorithm
{
};

/*! \brief Registry class for algorithms
 *
 *  The registry enables algorithms to be registered in a static variable
 */
class AlgorithmRegistry
{
public:
    using AlgorithmConstructor = std::function<Algorithm *()>;
    using Registry = std::unordered_map<std::string, AlgorithmConstructor>;

    //! Algorithm registration function, used internally by REGISTER_ALGORITHM macro
    static bool Register(const std::string &name, AlgorithmConstructor constructor);

    //! Access the registry of Algorithm registrated
    static Registry &GetRegistry();
};

} // namespace Core

// Add register macro for algorithms
#define REGISTER_ALGORITHM(ALGORITHM)                                                              \
    bool ALGORITHM##Algorithm =                                                                    \
        AlgorithmRegistry::Register(#ALGORITHM, []() { return new ALGORITHM(); });
