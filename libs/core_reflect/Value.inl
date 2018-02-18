#pragma once

#include "Value.h"


namespace RF { namespace reflect {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
Value::Value( T const& value )
	: m_InternalStorage( value )
{
	//
}



template<typename T>
typename std::remove_const<T>::type const* Value::GetAs() const
{
	using NonConstT = std::remove_const<T>::type;
	NonConstT const* retVal = std::get_if<NonConstT>( &m_InternalStorage );
	return retVal;
}



template<typename T>
constexpr Value::Type Value::DetermineType()
{
	using NonConstT = std::remove_const<T>::type;
	constexpr int64_t kIndexOfType = ValueTypes::FindIndex<NonConstT>::value;
	static_assert( kIndexOfType >= static_cast<int64_t>( Type::Invalid ), "Index below expected" );
	static_assert( kIndexOfType < static_cast<int64_t>( ValueTypes::kNumTypes ), "Index above expected" );
	return static_cast<Type>( kIndexOfType );
}

///////////////////////////////////////////////////////////////////////////////
}}
