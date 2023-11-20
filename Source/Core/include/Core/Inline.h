#pragma once

#ifdef __GNUC__
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif
