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
RFTYPE_DEFINE_STATIC_CLASSINFO( RF::rftype_example::ExampleWithStaticClassInfo );

RFTYPE_CREATE_META( RF::rftype_example::ExampleWithoutStaticClassInfo )
{
	using RF::rftype_example::ExampleWithoutStaticClassInfo;
	RFTYPE_META().Method( "ExampleNonStaticFunction", &ExampleWithoutStaticClassInfo::ExampleNonStaticFunction );
	RFTYPE_META().Method( "ExampleStaticFunction", &ExampleWithoutStaticClassInfo::ExampleStaticFunction );
	RFTYPE_META().RawProperty( "mExampleNonStaticVariable", &ExampleWithoutStaticClassInfo::mExampleNonStaticVariable );
	RFTYPE_META().RawProperty( "mExampleStaticVariable", &ExampleWithoutStaticClassInfo::mExampleStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleWithoutStaticClassInfo" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExampleWithClassAsMember )
{
	using RF::rftype_example::ExampleWithClassAsMember;
	RFTYPE_META().RawProperty( "mExampleClassAsMember", &ExampleWithClassAsMember::mExampleClassAsMember );
	RFTYPE_REGISTER_BY_NAME( "ExampleWithClassAsMember" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExampleBaseClass )
{
	using RF::rftype_example::ExampleBaseClass;
	RFTYPE_META().Method( "ExampleNonStaticFunction", &ExampleBaseClass::ExampleNonStaticFunction );
	RFTYPE_META().Method( "ExampleStaticFunction", &ExampleBaseClass::ExampleStaticFunction );
	RFTYPE_META().RawProperty( "mExampleNonStaticVariable", &ExampleBaseClass::mExampleNonStaticVariable );
	RFTYPE_META().RawProperty( "mExampleStaticVariable", &ExampleBaseClass::mExampleStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleBaseClass" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExampleDerivedClass )
{
	using RF::rftype_example::ExampleDerivedClass;
	RFTYPE_META().BaseClass<RF::rftype_example::ExampleBaseClass>();
	RFTYPE_META().RawProperty( "mExampleDerivedNonStaticVariable", &ExampleDerivedClass::mExampleDerivedNonStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleDerivedClass" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExampleSecondaryBaseClass )
{
	using RF::rftype_example::ExampleSecondaryBaseClass;
	RFTYPE_META().RawProperty( "mExampleSecondaryNonStaticVariable", &ExampleSecondaryBaseClass::mExampleSecondaryNonStaticVariable );
	RFTYPE_REGISTER_BY_NAME( "ExampleSecondaryBaseClass" );
}

RFTYPE_CREATE_META( RF::rftype_example::ExamplePoorLifeDecision )
{
	using RF::rftype_example::ExamplePoorLifeDecision;
	RFTYPE_META().BaseClass<RF::rftype_example::ExampleBaseClass>();
	RFTYPE_META().BaseClass<RF::rftype_example::ExampleSecondaryBaseClass>();
	RFTYPE_REGISTER_BY_NAME( "ExamplePoorLifeDecision" );
}

namespace RF { namespace rftype_example {
///////////////////////////////////////////////////////////////////////////////

bool ExampleWithStaticClassInfo::mExampleStaticVariable;
bool ExampleWithoutStaticClassInfo::mExampleStaticVariable;
bool ExampleBaseClass::mExampleStaticVariable;

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

bool ExampleBaseClass::ExampleNonStaticFunction( int i, float f, char c )
{
	return false;
}



bool ExampleBaseClass::ExampleStaticFunction( int i, float f, char c )
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}}
