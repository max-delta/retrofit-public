#include "stdafx.h"
#include "OverworldCharacter.h"

#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/OverworldVisual.h"
#include "cc3o3/state/components/OverworldMovement.h"

#include "Logging/Logging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"

namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateOverworldCharacterFromDB( state::VariableIdentifier const& objIdentifier, rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( objIdentifier );
	MakeOverworldCharacterFromDB( ref, charID );
	return ref;
}



void MakeOverworldCharacterFromDB( MutableObjectRef const& ref, rftl::immutable_string const& charID )
{
	// Visual
	{
		MutableComponentInstanceRefT<comp::OverworldVisual> const visual =
			ref.AddComponentInstanceT<comp::OverworldVisual>(
				DefaultCreator<comp::OverworldVisual>::Create() );
		RFLOG_TEST_AND_FATAL( visual != nullptr, nullptr, RFCAT_CC3O3, "Failed to add overworld visual component" );

		// TODO
		( (void)visual );
	}

	// Movement
	{
		MutableComponentInstanceRefT<comp::OverworldMovement> const move =
			ref.AddComponentInstanceT<comp::OverworldMovement>(
				DefaultCreator<comp::OverworldMovement>::Create() );
		RFLOG_TEST_AND_FATAL( move != nullptr, nullptr, RFCAT_CC3O3, "Failed to add overworld movement component" );

		// TODO
		( (void)move );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
