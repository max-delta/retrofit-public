#include "stdafx.h"
#include "CharData.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::CharData )
{
	using namespace RF::cc::character;
	RFTYPE_META().RawProperty( "mDescription", &CharData::mDescription );
	RFTYPE_META().ExtensionProperty( "mInnate", &CharData::mInnate );
	RFTYPE_META().RawProperty( "mGenetics", &CharData::mGenetics );
	RFTYPE_META().RawProperty( "mVisuals", &CharData::mVisuals );
	RFTYPE_META().RawProperty( "mStats", &CharData::mStats );
	RFTYPE_META().RawProperty( "mEquipment", &CharData::mEquipment );
	RFTYPE_META().RawProperty( "mInnateElements", &CharData::mInnateElements );
	RFTYPE_META().RawProperty( "mEquippedElements", &CharData::mEquippedElements );
	RFTYPE_REGISTER_BY_NAME( "CharData" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::CharData );
}


namespace RF { namespace cc { namespace character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}}}
