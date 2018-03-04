#include "stdafx.h"
#include "Example.h"

#include "RFType/CreateClassInfoDefinition.h"


RFTYPE_CREATE_META( RF::rftype_example::ExampleWithStaticClassInfo )
{
	using RF::rftype_example::ExampleWithStaticClassInfo;
	RFTYPE_META().Method( "ExampleNonStaticFunction", &ExampleWithStaticClassInfo::ExampleNonStaticFunction );
	RFTYPE_META().Method( "ExampleStaticFunction", &ExampleWithStaticClassInfo::ExampleStaticFunction );
	RFTYPE_META().RawProperty( "mExampleNonStaticVariable", &ExampleWithStaticClassInfo::mExampleNonStaticVariable );
	RFTYPE_META().RawProperty( "mExampleStaticVariable", &ExampleWithStaticClassInfo::mExampleStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleWithStaticClassInfo" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExampleWithoutStaticClassInfo )
{
	using RF::rftype_example::ExampleWithoutStaticClassInfo;
	RFTYPE_META().Method( "ExampleNonStaticFunction", &ExampleWithoutStaticClassInfo::ExampleNonStaticFunction );
	RFTYPE_META().Method( "ExampleStaticFunction", &ExampleWithoutStaticClassInfo::ExampleStaticFunction );
	RFTYPE_META().RawProperty( "mExampleNonStaticVariable", &ExampleWithoutStaticClassInfo::mExampleNonStaticVariable );
	RFTYPE_META().RawProperty( "mExampleStaticVariable", &ExampleWithoutStaticClassInfo::mExampleStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleWithoutStaticClassInfo" );
}

namespace RF { namespace rftype_example {
///////////////////////////////////////////////////////////////////////////////

bool ExampleWithStaticClassInfo::mExampleStaticVariable;
bool ExampleWithoutStaticClassInfo::mExampleStaticVariable;

///////////////////////////////////////////////////////////////////////////////

bool ExampleWithStaticClassInfo::ExampleNonStaticFunction( int i, float f, char c )
{
	return false;
}



bool ExampleWithStaticClassInfo::ExampleStaticFunction( int i, float f, char c )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////

bool ExampleWithoutStaticClassInfo::ExampleNonStaticFunction( int i, float f, char c )
{
	return false;
}



bool ExampleWithoutStaticClassInfo::ExampleStaticFunction( int i, float f, char c )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}
