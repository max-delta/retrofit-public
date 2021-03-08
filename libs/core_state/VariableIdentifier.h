#pragma once
#include "StateFwd.h"

#include "core_identifier/SegmentedIdentifier.h"


namespace RF::state {
///////////////////////////////////////////////////////////////////////////////

class VariableIdentifier : public id::SegmentedIdentifier<rftl::string>
{
	//
	// Public methods
public:
	VariableIdentifier()
		: SegmentedIdentifier() {}
	VariableIdentifier( SegmentedIdentifier<rftl::string> const& identifier )
		: SegmentedIdentifier( identifier ) {}
	template<typename... Nodes>
	explicit VariableIdentifier( Element element, Nodes... elements )
		: SegmentedIdentifier( element, elements... ) {}
};

///////////////////////////////////////////////////////////////////////////////
}

template<>
struct rftl::hash<RF::state::VariableIdentifier> : public rftl::hash<RF::id::SegmentedIdentifier<rftl::string>>
{
	//
};
