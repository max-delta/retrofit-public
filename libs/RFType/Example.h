#pragma once
#include "project.h"

#include "RFType/CreateClassInfoDeclaration.h"


namespace RF { namespace rftype_example {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Static class info not needed, but is similar to the pattern required
//  by other reflection systems
class RFTYPE_API ExampleWithStaticClassInfo
{
	RFTYPE_STATIC_CLASSINFO();

public:
	bool ExampleNonStaticFunction( int i, float f, char c );
	static bool ExampleStaticFunction( int i, float f, char c );

	bool mExampleNonStaticVariable;
	static bool mExampleStaticVariable;
};

///////////////////////////////////////////////////////////////////////////////

// NOTE: Dll export not required, but its absence means that the only way to
//  invoke its functions cross-dll will be via RFType
class /*RFTYPE_API*/ ExampleWithoutStaticClassInfo
{
public:
	bool ExampleNonStaticFunction( int i, float f, char c );
	static bool ExampleStaticFunction( int i, float f, char c );

	bool mExampleNonStaticVariable;
	static bool mExampleStaticVariable;
};

///////////////////////////////////////////////////////////////////////////////

// NOTE: Directly nested classes are supported without the need for indirection
class RFTYPE_API ExampleWithClassAsMember
{
public:
	ExampleWithoutStaticClassInfo mExampleClassAsMember;
};

///////////////////////////////////////////////////////////////////////////////

// NOTE: Deriving from VirtualClass allows run-time inspection on an instance
// NOTE: Enabling virtual lookup allows an instance to be properly identified,
//  where the absence of the lookup causes the instance to be interpreted as a
//  base class instead
class RFTYPE_API ExampleBaseClass : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

public:
	bool ExampleNonStaticFunction( int i, float f, char c );
	static bool ExampleStaticFunction( int i, float f, char c );

	bool mExampleNonStaticVariable;
	static bool mExampleStaticVariable;
};

class RFTYPE_API ExampleDerivedClass : public ExampleBaseClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

public:
	bool mExampleDerivedNonStaticVariable;
};

class RFTYPE_API ExampleSecondaryBaseClass : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

public:
	bool mExampleSecondaryNonStaticVariable;
};

// NOTE: Please... don't...
class RFTYPE_API ExamplePoorLifeDecision : virtual public ExampleBaseClass, virtual public ExampleSecondaryBaseClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();
};

///////////////////////////////////////////////////////////////////////////////
}}
