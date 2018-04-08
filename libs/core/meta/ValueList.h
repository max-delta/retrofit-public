#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueTypeT, ValueTypeT... Values>
struct ValueList;

// These MUST be fully outside of the class, or MSVC2017 compiler crashes
//  with an internal compilation error
namespace typelist_details
{
template<size_t Index, typename ValueListType>
struct ExternalAccessByIndex;
template<typename ValueType, ValueType Value, typename ValueListType>
struct ExternalAccessContains;
template<typename ValueType, ValueType Value, size_t CurrentIndex, typename ValueListType>
struct ExternalAccessFindIndex;
template<typename ValueType, ValueType Value, typename ValueListType>
struct ExternalAccessPrepend;
template<typename ValueType, ValueType Value, typename ValueListType>
struct ExternalAccessAppend;
template<typename ValueListType>
struct ExternalAccessReverse;
template<size_t FirstIndexOfSecondSegment, typename RemainingValueListType>
struct ExternalAccessSplitKeepLatter;
}


template<typename ValueTypeT, ValueTypeT... Values>
struct ValueList
{
	using ValueType = ValueTypeT;
	static_assert( rftl::is_class<ValueType>::value == false,
		"Not a simple type, expect compiler to explode with errors" );
	static constexpr size_t kNumValues = sizeof...( Values );

	// Used to access types by index
	template<size_t Index>
	struct ByIndex
	{
		static constexpr ValueType value = typelist_details::ExternalAccessByIndex< Index, ValueList<ValueType, Values...> >::value;
	};

	// Used to test for presence of a value
	template<ValueType Value>
	struct Contains
	{
		static constexpr bool value = typelist_details::ExternalAccessContains<ValueType, Value, ValueList<ValueType, Values...> >::value;
	};

	// Used to search for the index of a value
	template<ValueType Value>
	struct FindIndex
	{
		static constexpr int64_t value = typelist_details::ExternalAccessFindIndex<ValueType, Value, 0, ValueList<ValueType, Values...> >::value;
	};

	// Used to add to a list
	template<ValueType Value>
	struct Prepend
	{
		using type = typename typelist_details::ExternalAccessPrepend<ValueType, Value, ValueList<ValueType, Values...> >::type;
	};
	template<ValueType Value>
	struct Append
	{
		using type = typename typelist_details::ExternalAccessAppend<ValueType, Value, ValueList<ValueType, Values...> >::type;
	};

	// Used to reverse a list
	template<int Unused = 0>
	struct Reverse
	{
		using type = typename typelist_details::ExternalAccessReverse< ValueList<ValueType, Values...> >::type;
	};

	// Used to split a list into 2 segments
	template<size_t FirstIndexOfSecondSegment>
	struct Split
	{
	private:
		static constexpr size_t kReversedLastIndexOfFirstSegment = kNumValues - FirstIndexOfSecondSegment;
		using ReversedList = typename typelist_details::ExternalAccessReverse< ValueList<ValueType, Values...> >::type;
		using SplitReversedList = typename typelist_details::ExternalAccessSplitKeepLatter< kReversedLastIndexOfFirstSegment, ReversedList >::latter;
	public:
		using former = typename typelist_details::ExternalAccessReverse< SplitReversedList >::type;
		using latter = typename typelist_details::ExternalAccessSplitKeepLatter< FirstIndexOfSecondSegment, ValueList<ValueType, Values...> >::latter;
	};
};


namespace typelist_details
{
// 0->N-th case
// NOTE: This COULD be recursive to explicitly prevent value array
//  creation, but that crashes the MSVC2017 compiler
template<typename ValueType, size_t Index, ValueType... ValuesT>
struct ExternalAccessByIndex<Index, ValueList<ValueType, ValuesT...> >
{
	static constexpr ValueType values[] = { ValuesT... };
	static constexpr ValueType value = values[Index];
};

// 0 case
template<typename ValueType, ValueType Value>
struct ExternalAccessContains<ValueType, Value, ValueList<ValueType> >
{
	static constexpr bool value = false;
};

// N case
template<typename ValueType, ValueType Value, ValueType CurrentValue, ValueType... RemainingValues>
struct ExternalAccessContains<ValueType, Value, ValueList<ValueType, CurrentValue, RemainingValues...> >
{
	static constexpr bool kIsCurrent = Value == CurrentValue;
	static constexpr bool value = kIsCurrent ? true : ExternalAccessContains<ValueType, Value, ValueList<ValueType, RemainingValues...> >::value;
};

// 0 case
template<typename ValueType, ValueType Value, int64_t CurrentIndex>
struct ExternalAccessFindIndex<ValueType, Value, CurrentIndex, ValueList<ValueType> >
{
	static constexpr int64_t value = -1;
};

// N case
template<typename ValueType, ValueType Value, int64_t CurrentIndex, ValueType CurrentValue, ValueType... RemainingValues>
struct ExternalAccessFindIndex<ValueType, Value, CurrentIndex, ValueList<ValueType, CurrentValue, RemainingValues...> >
{
	static constexpr bool kIsCurrent = Value == CurrentValue;
	static constexpr int64_t value = kIsCurrent ? CurrentIndex : ExternalAccessFindIndex<ValueType, Value, CurrentIndex + 1, ValueList<ValueType, RemainingValues...> >::value;
};

// Single case
template<typename ValueType, ValueType Value, ValueType... ExistingValues>
struct ExternalAccessPrepend<ValueType, Value, ValueList<ValueType, ExistingValues...> >
{
	using type = ValueList<ValueType, Value, ExistingValues...>;
};

// Single case
template<typename ValueType, ValueType Value, ValueType... ExistingValues>
struct ExternalAccessAppend<ValueType, Value, ValueList<ValueType, ExistingValues...> >
{
	using type = ValueList<ValueType, ExistingValues..., Value>;
};

// Zero-th case
template<typename ValueType>
struct ExternalAccessReverse<ValueList<ValueType> >
{
	using type = ValueList<ValueType>;
};

// N-th case
template<typename ValueType, ValueType CurrentValue, ValueType... RemainingValues>
struct ExternalAccessReverse<ValueList<ValueType, CurrentValue, RemainingValues...> >
{
	using RemainingTypeList = ValueList<ValueType, RemainingValues...>;
	using RemainingTypeListReversed = typename ExternalAccessReverse<RemainingTypeList>::type;
	using type = typename ExternalAccessAppend<ValueType, CurrentValue, RemainingTypeListReversed>::type;
};

// Zero-th case
template<typename ValueType>
struct ExternalAccessSplitKeepLatter<0, ValueList<ValueType> >
{
	using latter = ValueList<ValueType>;
};

// Zero-th case
template<typename ValueType, ValueType CurrentValue, ValueType... RemainingValues>
struct ExternalAccessSplitKeepLatter<0, ValueList<ValueType, CurrentValue, RemainingValues...> >
{
	using latter = ValueList<ValueType, CurrentValue, RemainingValues...>;
};

// N-th case
template<typename ValueType, size_t FirstIndexOfSecondSegment, ValueType CurrentValue, ValueType... RemainingValues>
struct ExternalAccessSplitKeepLatter<FirstIndexOfSecondSegment, ValueList<ValueType, CurrentValue, RemainingValues...> >
{
	static_assert( FirstIndexOfSecondSegment - 1 <= sizeof...( RemainingValues ), "Attempting to split past the end of type list" );
	using latter = typename ExternalAccessSplitKeepLatter< FirstIndexOfSecondSegment - 1, ValueList<ValueType, RemainingValues...> >::latter;
};
}

///////////////////////////////////////////////////////////////////////////////
}
