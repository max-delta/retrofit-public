#include "stdafx.h"
#include "Stats.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/string.h"


RFTYPE_CREATE_META( RF::cc::character::Stats )
{
	using namespace RF::cc::character;
	RFTYPE_META().RawProperty( "mMHealth", &Stats::mMHealth );
	RFTYPE_META().RawProperty( "mPhysAtk", &Stats::mPhysAtk );
	RFTYPE_META().RawProperty( "mPhysDef", &Stats::mPhysDef );
	RFTYPE_META().RawProperty( "mElemAtk", &Stats::mElemAtk );
	RFTYPE_META().RawProperty( "mElemDef", &Stats::mElemDef );
	RFTYPE_META().RawProperty( "mBalance", &Stats::mBalance );
	RFTYPE_META().RawProperty( "mTechniq", &Stats::mTechniq );
	RFTYPE_META().RawProperty( "mElemPwr", &Stats::mElemPwr );
	RFTYPE_META().RawProperty( "mGridShp", &Stats::mGridShpReflect );
	RFTYPE_REGISTER_BY_NAME( "Stats" );
	RFTYPE_REGISTER_BY_QUALIFIED_NAME( RF::cc::character::Stats );
}


namespace RF::cc::character {
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
}
