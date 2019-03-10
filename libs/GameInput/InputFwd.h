#pragma once
#include "project.h"


namespace RF { namespace input {
///////////////////////////////////////////////////////////////////////////////

using RawCommandType = uint64_t;
constexpr RawCommandType kInvalidRawCommand = 0;
using RawSignalType = uint64_t;
constexpr RawSignalType kInvalidRawSignal = 0;
using RawSignalValue = float;

using GameCommandType = uint64_t;
constexpr GameCommandType kInvalidGameCommand = 0;
using GameSignalType = uint64_t;
constexpr GameSignalType kInvalidGameSignal = 0;
using GameSignalValue = float;

class RawController;
class GameController;
class RawInputController;

///////////////////////////////////////////////////////////////////////////////
}}
