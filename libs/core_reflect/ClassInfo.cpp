#include "stdafx.h"
#include "ClassInfo.h"

#include "core/rf_assert.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

bool ClassInfo::IsSameOrDerivedFrom( ClassInfo const& ancestor ) const
{
	if( &ancestor == this )
	{
		return true;
	}

	// Recurse to avoid heap changes, assuming inheritance chains won't be out
	//  of control in any decent engine
	for( BaseClassInfo const& baseClass : mBaseTypes )
	{
		ClassInfo const* const parent = baseClass.mBaseClassInfo;
		RF_ASSERT( parent != nullptr );
		if( parent->IsSameOrDerivedFrom( ancestor ) )
		{
			return true;
		}
	}

	return false;
}



void const* ClassInfo::AttemptInheritanceWalk( ClassInfo const& ancestor, void const* source ) const
{
	if( &ancestor == this )
	{
		return source;
	}

	// Recurse to avoid heap changes, assuming inheritance chains won't be out
	//  of control in any decent engine
	for( BaseClassInfo const& baseClass : mBaseTypes )
	{
		BaseClassInfo::FuncPtrGetBasePointerFromDerived const chainFunc = baseClass.mGetBasePointerFromDerived;
		void const* chain;
		if( chainFunc != nullptr )
		{
			chain = chainFunc( source );
		}
		else
		{
			chain = source;
		}

		ClassInfo const* const parent = baseClass.mBaseClassInfo;
		RF_ASSERT( parent != nullptr );
		void const* const end = parent->AttemptInheritanceWalk( ancestor, chain );
		if( end != nullptr )
		{
			return end;
		}
	}

	return nullptr;
}



char const* ClassInfo::StoreString( char const* string )
{
	rftl::string const& storedString = mIdentifierStorage.emplace_back( string );
	return storedString.data();
}

///////////////////////////////////////////////////////////////////////////////
}}
