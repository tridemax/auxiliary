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

#define EnumCompare(inputString, comparedLiteral) \
	(inputString.size() == sizeof(comparedLiteral) - 1u && memcmp(inputString.data(), comparedLiteral, sizeof(comparedLiteral) - 1u) == 0)

//-------------------------------------------------------------------------------------------------
/// auxiliary
//-------------------------------------------------------------------------------------------------
#include "source/Miscellaneous.h"
#include "source/Hash.h"
#include "source/FixedArray.h"
#include "source/FixedStream.h"
#include "source/VectorStream.h"
#include "source/ChunkedStorage.h"
#include "source/Clock.h"
#include "source/FileSystemUtils.h"
