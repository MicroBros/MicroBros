#pragma once

#include <type_traits>

#define BITFLAGS(IDENT)                                                \
    inline IDENT operator|(IDENT lhs, IDENT rhs)                       \
    {                                                                  \
        return static_cast<IDENT>(                                     \
            static_cast<std::underlying_type_t<IDENT>>(lhs) |          \
            static_cast<std::underlying_type_t<IDENT>>(rhs));          \
    }                                                                  \
    inline bool operator&(IDENT lhs, IDENT rhs)                        \
    {                                                                  \
        return (static_cast<std::underlying_type_t<IDENT>>(lhs) &      \
                static_cast<std::underlying_type_t<IDENT>>(rhs)) != 0; \
    }
