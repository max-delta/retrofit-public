#pragma once
#include "project.h"


namespace RF::input {
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

using PlayerID = uint8_t;
constexpr PlayerID kInvalidPlayerID = 0;
constexpr PlayerID kMaxPlayerID = 255;
using LayerID = uint8_t;
constexpr LayerID kInvalidLayerID = 0;
constexpr LayerID kMaxLayerID = 0;

struct RawCommand;
struct GameCommand;
struct RawSignal;
struct GameSignal;

class RawController;
class GameController;
class RawInputController;
class HotkeyController;
class ControllerManager;

///////////////////////////////////////////////////////////////////////////////
}
