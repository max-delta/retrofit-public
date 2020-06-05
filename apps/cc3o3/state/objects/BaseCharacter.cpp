#include "stdafx.h"
#include "BaseCharacter.h"

#include "cc3o3/Common.h"
#include "cc3o3/char/CharacterDatabase.h"
#include "cc3o3/state/ComponentResolver.h"
#include "cc3o3/state/objects/EmptyObject.h"
#include "cc3o3/state/components/Character.h"
#include "cc3o3/state/StateLogging.h"

#include "core_component/TypedObjectRef.h"
#include "core_component/TypedComponentRef.h"

#include "core/ptr/default_creator.h"

namespace RF::cc::state::obj {
///////////////////////////////////////////////////////////////////////////////

MutableObjectRef CreateBaseCharacterFromDB(
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	rftl::immutable_string const& charID )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeBaseCharacterFromDB( sharedWindow, privateWindow, ref, charID );
	return ref;
}



void MakeBaseCharacterFromDB(
	rollback::Window& sharedWindow,
	rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	rftl::immutable_string const& charID )
{
	// Character
	{
		MutableComponentInstanceRefT<comp::Character> const chara =
			ref.AddComponentInstanceT<comp::Character>(
				DefaultCreator<comp::Character>::Create() );
		RFLOG_TEST_AND_FATAL( chara != nullptr, ref, RFCAT_CC3O3, "Failed to add character component" );

		character::CharacterDatabase const& charDB = *gCharacterDatabase;
		chara->mCharacter = charDB.FetchExistingCharacter( charID );
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as base character" );
}

///////////////////////////////////////////////////////////////////////////////
}
