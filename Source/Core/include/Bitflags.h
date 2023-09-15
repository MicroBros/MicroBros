#pragma once

#include <type_traits>

#define BITFLAGS_BEGIN(IDENT, TYPE)                                                                \
    struct IDENT                                                                                   \
    {                                                                                              \
        enum Flags : TYPE                                                                          \
        {

#define BITFLAGS_END(IDENT)                                                                        \
    }                                                                                              \
    ;                                                                                              \
                                                                                                   \
private:                                                                                           \
    using ValueType = std::underlying_type_t<Flags>;                                               \
    ValueType value;                                                                               \
    IDENT(ValueType value) : value{value} {}                                                       \
                                                                                                   \
public:                                                                                            \
    IDENT() : value{0} {}                                                                          \
    IDENT(Flags value) : value{value} {}                                                           \
                                                                                                   \
    inline IDENT operator|(IDENT rhs) const noexcept                                               \
    {                                                                                              \
        return IDENT{static_cast<ValueType>(value | rhs.value)};                                   \
    }                                                                                              \
    inline IDENT &operator|=(const IDENT rhs) noexcept                                             \
    {                                                                                              \
        value = value | rhs.value;                                                                 \
        return *this;                                                                              \
    }                                                                                              \
    inline IDENT operator&(const IDENT rhs) const noexcept                                         \
    {                                                                                              \
        return IDENT{static_cast<ValueType>(value & rhs.value)};                                   \
    }                                                                                              \
    inline IDENT &operator&=(const IDENT rhs) noexcept                                             \
    {                                                                                              \
        value = value & rhs.value;                                                                 \
        return *this;                                                                              \
    }                                                                                              \
    inline IDENT operator~() const noexcept { return IDENT(~value); }                              \
    inline bool Contains(const IDENT value) const noexcept                                         \
    {                                                                                              \
        return (this->value & value.value) == value.value;                                         \
    }                                                                                              \
    }                                                                                              \
    ;
