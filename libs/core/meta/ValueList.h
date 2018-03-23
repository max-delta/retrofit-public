#pragma once
#include <stdint.h>

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueTypeT, ValueTypeT... Values>
struct ValueList;

// These MUST be fully outside of the class, or MSVC2017 compiler crashes
//  with an internal compilation error
namespace typelist_details
{
template<size_t FirstIndexOfSecondSegment, typename RemainingValueListType>
struct ExternalAccessSplitKeepLatter;

// Zero-th case
template<typename ValueTypeT>
struct ExternalAccessSplitKeepLatter<0, ValueList<ValueTypeT> >
{
	using latter = ValueList<ValueTypeT>;
};

// Zero-th case
template<typename ValueTypeT, ValueTypeT CurrentValue, ValueTypeT... RemainingValues>
struct ExternalAccessSplitKeepLatter<0, ValueList<ValueTypeT, CurrentValue, RemainingValues...> >
{
	using latter = ValueList<ValueTypeT, CurrentValue, RemainingValues...>;
};

// N-th case
template<typename ValueTypeT, size_t FirstIndexOfSecondSegment, ValueTypeT CurrentValue, ValueTypeT... RemainingValues>
struct ExternalAccessSplitKeepLatter<FirstIndexOfSecondSegment, ValueList<ValueTypeT, CurrentValue, RemainingValues...> >
{
	static_assert( FirstIndexOfSecondSegment - 1 <= sizeof...( RemainingValues ), "Attempting to split past the end of type list" );
	using latter = typename ExternalAccessSplitKeepLatter< FirstIndexOfSecondSegment - 1, ValueList<ValueTypeT, RemainingValues...> >::latter;
};
}

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

	// Used to add to a list
	template<ValueType Value>
	struct Prepend;
	template<ValueType Value>
	struct Append;

	// Used to reverse a list
	template<int Unused = 0>
	struct Reverse;

	// Used to split a list into 2 segments
	template<size_t FirstIndexOfSecondSegment>
	struct Split;


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
	template<ValueType Value, typename ValueListType>
	struct ExternalAccessPrepend;
	template<ValueType Value, typename ValueListType>
	struct ExternalAccessAppend;
	template<typename ValueListType>
	struct ExternalAccessReverse;

	// 0->N-th case
	// NOTE: This COULD be recursive to explicitly prevent value array
	//  creation, but that crashes the MSVC2017 compiler
	template<size_t Index, ValueType... ValuesT>
	struct ExternalAccessByIndex<Index, ValueList<ValueType, ValuesT...> >
	{
		static constexpr ValueType values[] = { ValuesT... };
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

	// Single case
	template<ValueType Value, ValueType... ExistingValues>
	struct ExternalAccessPrepend<Value, ValueList<ValueType, ExistingValues...> >
	{
		using type = ValueList<ValueType, Value, ExistingValues...>;
	};

	// Single case
	template<ValueType Value, ValueType... ExistingValues>
	struct ExternalAccessAppend<Value, ValueList<ValueType, ExistingValues...> >
	{
		using type = ValueList<ValueType, ExistingValues..., Value>;
	};

	// Zero-th case
	template<>
	struct ExternalAccessReverse<ValueList<ValueType> >
	{
		using type = ValueList<ValueType>;
	};

	// N-th case
	template<ValueType CurrentValue, ValueType... RemainingValues>
	struct ExternalAccessReverse<ValueList<ValueType, CurrentValue, RemainingValues...> >
	{
		using RemainingTypeList = ValueList<ValueType, RemainingValues...>;
		using RemainingTypeListReversed = typename ExternalAccessReverse<RemainingTypeList>::type;
		using type = typename ExternalAccessAppend<CurrentValue, RemainingTypeListReversed>::type;
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

	// Implemented as external
	template<ValueType Value>
	struct Prepend : ExternalAccessPrepend<Value, ValueList<ValueType, Values...> >
	{
		//
	};

	// Implemented as external
	template<ValueType Value>
	struct Append : ExternalAccessAppend<Value, ValueList<ValueType, Values...> >
	{
		//
	};

	// Implemented as external
	template<int Unused>
	struct Reverse : ExternalAccessReverse< ValueList<ValueType, Values...> >
	{
		//
	};

	// Implemented as external
	template<size_t FirstIndexOfSecondSegment>
	struct Split
	{
		static constexpr size_t kReversedLastIndexOfFirstSegment = kNumValues - FirstIndexOfSecondSegment;
		using ReversedList = typename ExternalAccessReverse< ValueList<ValueType, Values...> >::type;
		using SplitReversedList = typename typelist_details::ExternalAccessSplitKeepLatter< kReversedLastIndexOfFirstSegment, ReversedList >::latter;
		using former = typename ExternalAccessReverse< SplitReversedList >::type;
		using latter = typename typelist_details::ExternalAccessSplitKeepLatter< FirstIndexOfSecondSegment, ValueList<ValueType, Values...> >::latter;
	};
};

///////////////////////////////////////////////////////////////////////////////
}
