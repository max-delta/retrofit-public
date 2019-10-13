#pragma once
#include "rftl/string"

namespace RF { namespace state {
///////////////////////////////////////////////////////////////////////////////

// TODO: This should be a class instead that has scope knowledge
using VariableIdentifier = rftl::string;

template<typename ValueT>
struct StateChange;

template<typename ValueT, size_t MaxChangesT>
class StateStream;

template<size_t MaxChangesT>
class StateBag;

///////////////////////////////////////////////////////////////////////////////
}}
