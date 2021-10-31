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

MutableObjectRef CreateBaseCharacter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	character::CharData const& charData )
{
	MutableObjectRef const ref = CreateEmptyObject( sharedWindow, privateWindow, objIdentifier );
	MakeBaseCharacter( sharedWindow, privateWindow, ref, charData );
	return ref;
}



void MakeBaseCharacter(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	character::CharData const& charData )
{
	// Character
	{
		MutableComponentInstanceRefT<comp::Character> const chara =
			ref.AddComponentInstanceT<comp::Character>(
				DefaultCreator<comp::Character>::Create() );
		RFLOG_TEST_AND_FATAL( chara != nullptr, ref, RFCAT_CC3O3, "Failed to add character component" );
		chara->BindToMeta( sharedWindow, privateWindow, ref );

		chara->mCharData = charData;
	}

	RFLOG_DEBUG( ref, RFCAT_CC3O3, "Prepared as base character" );
}



MutableObjectRef CreateBaseCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	state::VariableIdentifier const& objIdentifier,
	rftl::immutable_string const& charID )
{
	character::CharacterDatabase const& charDB = *gCharacterDatabase;
	character::CharData const charData = charDB.FetchExistingCharacter( charID );
	return CreateBaseCharacter( sharedWindow, privateWindow, objIdentifier, charData );
}



void MakeBaseCharacterFromDB(
	rollback::Window& sharedWindow, rollback::Window& privateWindow,
	MutableObjectRef const& ref,
	rftl::immutable_string const& charID )
{
	character::CharacterDatabase const& charDB = *gCharacterDatabase;
	character::CharData const charData = charDB.FetchExistingCharacter( charID );
	MakeBaseCharacter( sharedWindow, privateWindow, ref, charData );
}

///////////////////////////////////////////////////////////////////////////////
}
