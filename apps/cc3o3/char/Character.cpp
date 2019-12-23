#include "stdafx.h"
#include "Character.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Character )
{
	using namespace RF::cc::character;
	RFTYPE_META().RawProperty( "mDescription", &Character::mDescription );
	RFTYPE_META().ExtensionProperty( "mInnate", &Character::mInnate );
	RFTYPE_META().RawProperty( "mGenetics", &Character::mGenetics );
	RFTYPE_META().RawProperty( "mVisuals", &Character::mVisuals );
	RFTYPE_META().RawProperty( "mStats", &Character::mStats );
	RFTYPE_META().RawProperty( "mEquipment", &Character::mEquipment );
	RFTYPE_META().RawProperty( "mInnateElements", &Character::mInnateElements );
	RFTYPE_META().RawProperty( "mEquippedElements", &Character::mEquippedElements );
	RFTYPE_REGISTER_BY_NAME( "Character" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Character );
}


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}}}
