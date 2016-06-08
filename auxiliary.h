#pragma once

//-------------------------------------------------------------------------------------------------
/// byte type declaration
//-------------------------------------------------------------------------------------------------
typedef uint8_t byte;

static_assert(sizeof(byte) == 1u, "Size of \"byte\" typedef not equal to 8 bit.");

//-------------------------------------------------------------------------------------------------
/// defines
//-------------------------------------------------------------------------------------------------
#define forceinline			inline __attribute__((always_inline))
#define selectany			__attribute__((weak))

//-------------------------------------------------------------------------------------------------
/// auxiliary
//-------------------------------------------------------------------------------------------------
#include "auxiliary/Miscellaneous.h"
#include "auxiliary/Clock.h"
#include "auxiliary/FixedArray.h"
#include "auxiliary/FixedStream.h"
#include "auxiliary/VectorStream.h"
#include "google/farmhash.h"
