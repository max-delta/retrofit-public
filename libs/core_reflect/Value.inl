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
	constexpr size_t kIndexOfType = ValueTypes::FindIndex<T>::value;
	return static_cast<Type>( kIndexOfType );
}

///////////////////////////////////////////////////////////////////////////////
}}
