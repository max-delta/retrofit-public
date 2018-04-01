#pragma once

#include "ClassInfoBuilder.h"

#include "core/compiler.h"


namespace RF { namespace reflect { namespace builder {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename TypeListType>
struct ParamUnpacker;
// 0 case
template<>
struct ParamUnpacker<TypeList<> >
{
	static void Unpack( std::vector<Value::Type>& paramTypes )
	{
	}
};
// N case
template<typename CurrentType, typename... RemainingTypes>
struct ParamUnpacker<TypeList<CurrentType, RemainingTypes...> >
{
	static void Unpack( std::vector<Value::Type>& paramTypes )
	{
		constexpr Value::Type kType = Value::DetermineType<CurrentType>();
		paramTypes.push_back( kType );
		ParamUnpacker<TypeList<RemainingTypes...> >::Unpack( paramTypes );
	}
};

}
///////////////////////////////////////////////////////////////////////////////

template <class T>
void CreateClassInfo( ClassInfo& classInfo )
{
	static_assert( std::is_class<T>::value, "CreateClassInfo requires a class" );
	classInfo = {};
	classInfo.mIsPolymorphic = std::is_polymorphic<T>::value;
	classInfo.mIsAbstract = std::is_abstract<T>::value;
	classInfo.mIsDefaultConstructible = std::is_default_constructible<T>::value;
	classInfo.mIsCopyConstructible = std::is_copy_constructible<T>::value;
	classInfo.mIsMoveConstructible = std::is_move_constructible<T>::value;
	classInfo.mIsDestructible = std::is_default_constructible<T>::value;
	classInfo.mHasVirtualDestructor = std::has_virtual_destructor<T>::value;
	classInfo.mMinimumAlignment = std::alignment_of<T>::value;
}



template<typename T>
void CreateFreeStandingVariableInfo( FreeStandingVariableInfo & variableInfo, T * variable )
{
	variableInfo = {};
	variableInfo.mAddress = reinterpret_cast<void const*>( static_cast<T*>( variable ) );
	variableInfo.mMutable = std::is_const<T>::value == false;
	variableInfo.mSize = sizeof( T );
	variableInfo.mVariableTypeInfo.mValueType = Value::DetermineType<T>();
	variableInfo.mVariableTypeInfo.mClassInfo = nullptr;
	variableInfo.mVariableTypeInfo.mAccessor = nullptr;
}



template<typename T, typename Class>
void CreateMemberVariableInfo( MemberVariableInfo & variableInfo, T Class::* variable )
{
	variableInfo = {};
	Class const* const kInvalidClass = reinterpret_cast<Class const*>( compiler::kInvalidNonNullPointer );

	// NOTE: In some inheritance implementations, the act of referencing an
	//  invalid pointer creates run-time code that will crash, so you will not
	//  be able to use this reflection helper on it, and will need to use more
	//  expensive accessors instead, that can perform the needed run-time code
	T const* const offsetIn = &( ( *kInvalidClass ).*variable );
	variableInfo.mOffset = reinterpret_cast<ptrdiff_t>( offsetIn ) - reinterpret_cast<ptrdiff_t>( kInvalidClass );
	variableInfo.mMutable = std::is_const<T>::value == false;
	variableInfo.mSize = sizeof( T );
	variableInfo.mVariableTypeInfo.mValueType = Value::DetermineType<T>();
	variableInfo.mVariableTypeInfo.mClassInfo = nullptr;
	variableInfo.mVariableTypeInfo.mAccessor = nullptr;
}



template<typename T>
void CreateFreeStandingFunctionInfo( FreeStandingFunctionInfo & functionInfo, T & function )
{
	functionInfo = {};
	functionInfo.mAddress = function;
	using FT = FunctionTraits<T>;
	functionInfo.mReturn.mValueType = Value::DetermineType<FT::ReturnType>();
	{
		std::vector<Value::Type> paramTypes;
		paramTypes.reserve( FT::ParamTypes::kNumTypes );
		details::ParamUnpacker<FT::ParamTypes>::Unpack( paramTypes );
		functionInfo.Parameters.reserve( FT::ParamTypes::kNumTypes );
		for( size_t i = 0; i < paramTypes.size(); i++ )
		{
			ParameterInfo param = {};
			param.mValueType = paramTypes[i];
			functionInfo.Parameters.emplace_back( param );
		}
	}
}



template<typename T>
void CreateMemberFunctionInfo( MemberFunctionInfo & functionInfo, T function )
{
	functionInfo = {};
	using FT = FunctionTraits<T>;
	functionInfo.mRequiresConst = FT::kRequiresConst;
	functionInfo.mReturn.mValueType = Value::DetermineType<FT::ReturnType>();
	{
		std::vector<Value::Type> paramTypes;
		paramTypes.reserve( FT::ParamTypes::kNumTypes );
		details::ParamUnpacker<FT::ParamTypes>::Unpack( paramTypes );
		functionInfo.Parameters.reserve( FT::ParamTypes::kNumTypes );
		for( size_t i = 0; i < paramTypes.size(); i++ )
		{
			ParameterInfo param = {};
			param.mValueType = paramTypes[i];
			functionInfo.Parameters.emplace_back( param );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}}
