#pragma once

#include "Value.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
Value::Value( T const& value )
	: mInternalStorage( value )
{
	//
}



template<typename T>
typename rftl::remove_const<T>::type const* Value::GetAs() const
{
	using NonConstT = rftl::remove_const<T>::type;
	NonConstT const* retVal = rftl::get_if<NonConstT>( &mInternalStorage );
	return retVal;
}



template<typename T>
constexpr Value::Type Value::DetermineType()
{
	using NonConstT = rftl::remove_const<T>::type;
	constexpr int64_t kIndexOfType = ValueTypes::FindIndex<NonConstT>::value;
	static_assert( kIndexOfType >= static_cast<int64_t>( Type::Invalid ), "Index below expected" );
	static_assert( kIndexOfType < static_cast<int64_t>( ValueTypes::kNumTypes ), "Index above expected" );
	return static_cast<Type>( kIndexOfType );
}

///////////////////////////////////////////////////////////////////////////////
}}
