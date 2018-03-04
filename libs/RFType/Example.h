#pragma once
#include "project.h"

#include "RFType/CreateClassInfoDeclaration.h"

namespace RF { namespace rftype_example {
///////////////////////////////////////////////////////////////////////////////

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

class RFTYPE_API ExampleWithoutStaticClassInfo
{
public:
	bool ExampleNonStaticFunction( int i, float f, char c );
	static bool ExampleStaticFunction( int i, float f, char c );

	bool mExampleNonStaticVariable;
	static bool mExampleStaticVariable;
};

///////////////////////////////////////////////////////////////////////////////
}}
