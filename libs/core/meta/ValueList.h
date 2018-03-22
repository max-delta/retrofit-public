#pragma once
#include <stdint.h>

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueTypeT, ValueTypeT... Values>
struct ValueList
{
	using ValueType = ValueTypeT;
	static_assert( std::is_class<ValueType>::value == false,
		"Not a simple type, expect compiler to explode with errors" );
	static constexpr size_t kNumValues = sizeof...( Values );

	// Used to access types by index
	template<size_t Index>
	struct ByIndex;

	// Used to test for presence of a value
	template<ValueType Value>
	struct Contains;

	// Used to search for the index of a value
	template<ValueType Value>
	struct FindIndex;


private:
	// For external access
	// NOTE: Not actually available externally, but the pattern illustrates how
	//  to build other logic for unpacking the value list
	template<size_t Index, typename ValueListType>
	struct ExternalAccessByIndex;
	template<ValueType Value, typename ValueListType>
	struct ExternalAccessContains;
	template<ValueType Value, size_t CurrentIndex, typename ValueListType>
	struct ExternalAccessFindIndex;

	// 0->N-th case
	// NOTE: This COULD be recursive to explicitly prevent value array
	//  creation, but that crashes the MSVC2017 compiler
	template<size_t Index, ValueType... Values>
	struct ExternalAccessByIndex<Index, ValueList<ValueType, Values...> >
	{
		static constexpr ValueType values[] = { Values... };
		static constexpr ValueType value = values[Index];
	};

	// 0 case
	template<ValueType Value>
	struct ExternalAccessContains<Value, ValueList<ValueType> >
	{
		static constexpr bool value = false;
	};

	// N case
	template<ValueType Value, ValueType CurrentValue, ValueType... RemainingValues>
	struct ExternalAccessContains<Value, ValueList<ValueType, CurrentValue, RemainingValues...> >
	{
		static constexpr bool kIsCurrent = Value == CurrentValue;
		static constexpr bool value = kIsCurrent ? true : ExternalAccessContains<Value, ValueList<ValueType, RemainingValues...> >::value;
	};

	// 0 case
	template<ValueType Value, int64_t CurrentIndex>
	struct ExternalAccessFindIndex<Value, CurrentIndex, ValueList<ValueType> >
	{
		static constexpr int64_t value = -1;
	};

	// N case
	template<ValueType Value, int64_t CurrentIndex, ValueType CurrentValue, ValueType... RemainingValues>
	struct ExternalAccessFindIndex<Value, CurrentIndex, ValueList<ValueType, CurrentValue, RemainingValues...> >
	{
		static constexpr bool kIsCurrent = Value == CurrentValue;
		static constexpr int64_t value = kIsCurrent ? CurrentIndex : ExternalAccessFindIndex<Value, CurrentIndex + 1, ValueList<ValueType, RemainingValues...> >::value;
	};


public:
	// Implemented as external
	template<size_t Index>
	struct ByIndex : ExternalAccessByIndex < Index, ValueList<ValueType, Values...>>
	{
		//
	};

	// Implemented as external
	template<ValueType Value>
	struct Contains : ExternalAccessContains< Value, ValueList<ValueType, Values...> >
	{
		//
	};

	// Implemented as external
	template<ValueType Value>
	struct FindIndex : ExternalAccessFindIndex< Value, 0, ValueList<ValueType, Values...> >
	{
		//
	};
};

///////////////////////////////////////////////////////////////////////////////
}
