#pragma once

#include "rftl/type_traits"
#include "rftl/cstdint"

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename... Types>
struct TypeList;

// These SHOULD be fully outside of the class, but MSVC2017 compiler has
//  non-standard behavior that could premit them to be inside
namespace typelist_details {
template<size_t Index, typename TypeListType>
struct ExternalAccessByIndex;
template<typename Type, typename TypeListType>
struct ExternalAccessContains;
template<typename Type, size_t CurrentIndex, typename TypeListType>
struct ExternalAccessFindIndex;
template<typename Type, typename TypeListType>
struct ExternalAccessPrepend;
template<typename Type, typename TypeListType>
struct ExternalAccessAppend;
template<typename TypeListType>
struct ExternalAccessReverse;
template<size_t FirstIndexOfSecondSegment, typename RemainingTypeListType>
struct ExternalAccessSplitKeepLatter;
template<size_t FirstIndexOfSecondSegment, typename TypeListType>
struct ExternalAccessSplit;
}


template<typename... Types>
struct TypeList
{
	static constexpr size_t kNumTypes = sizeof...( Types );

	// Used to access types by index
	template<size_t Index>
	struct ByIndex
	{
		using type = typename typelist_details::ExternalAccessByIndex<Index, TypeList<Types...>>::type;
	};

	// Used to test for presence of a type
	template<typename Type>
	struct Contains
	{
		static constexpr bool value = typelist_details::ExternalAccessContains<Type, TypeList<Types...>>::value;
	};

	// Used to search for the index of a type
	template<typename Type>
	struct FindIndex
	{
		static constexpr int64_t value = typelist_details::ExternalAccessFindIndex<Type, 0, TypeList<Types...>>::value;
	};

	// Used to add to a list
	template<typename Type>
	struct Prepend
	{
		using type = typename typelist_details::ExternalAccessPrepend<Type, TypeList<Types...>>::type;
	};
	template<typename Type>
	struct Append
	{
		using type = typename typelist_details::ExternalAccessAppend<Type, TypeList<Types...>>::type;
	};

	// Used to reverse a list
	template<int Unused = 0>
	struct Reverse
	{
		using type = typename typelist_details::ExternalAccessReverse<TypeList<Types...>>::type;
	};

	// Used to split a list into 2 segments
	template<size_t FirstIndexOfSecondSegment>
	struct Split
	{
		using former = typename typelist_details::ExternalAccessSplit<FirstIndexOfSecondSegment, TypeList<Types...>>::former;
		using latter = typename typelist_details::ExternalAccessSplit<FirstIndexOfSecondSegment, TypeList<Types...>>::latter;
	};
};


namespace typelist_details {
// Zero-th case
template<typename CurrentType, typename... RemainingTypes>
struct ExternalAccessByIndex<0, TypeList<CurrentType, RemainingTypes...>>
{
	using type = CurrentType;
};

// N-th case
template<size_t Index, typename CurrentType, typename... RemainingTypes>
struct ExternalAccessByIndex<Index, TypeList<CurrentType, RemainingTypes...>>
{
	static_assert( Index - 1 < sizeof...( RemainingTypes ), "Attempting to index past the end of type list" );
	using type = typename ExternalAccessByIndex<Index - 1, TypeList<RemainingTypes...>>::type;
};

// 0 case
template<typename Type>
struct ExternalAccessContains<Type, TypeList<>>
{
	static constexpr bool value = false;
};

// N case
template<typename Type, typename CurrentType, typename... RemainingTypes>
struct ExternalAccessContains<Type, TypeList<CurrentType, RemainingTypes...>>
{
	using IsCurrent = rftl::is_same<Type, CurrentType>;
	static constexpr bool value = rftl::disjunction<IsCurrent, ExternalAccessContains<Type, TypeList<RemainingTypes...>>>::value;
};

// 0 case
template<typename Type, int64_t CurrentIndex>
struct ExternalAccessFindIndex<Type, CurrentIndex, TypeList<>>
{
	static constexpr int64_t value = -1;
};

// N case
template<typename Type, int64_t CurrentIndex, typename CurrentType, typename... RemainingTypes>
struct ExternalAccessFindIndex<Type, CurrentIndex, TypeList<CurrentType, RemainingTypes...>>
{
	using IsCurrent = rftl::is_same<Type, CurrentType>;
	static constexpr int64_t value = IsCurrent::value ? CurrentIndex : ExternalAccessFindIndex<Type, CurrentIndex + 1, TypeList<RemainingTypes...>>::value;
};

// Single case
template<typename Type, typename... ExistingTypes>
struct ExternalAccessPrepend<Type, TypeList<ExistingTypes...>>
{
	using type = TypeList<Type, ExistingTypes...>;
};

// Single case
template<typename Type, typename... ExistingTypes>
struct ExternalAccessAppend<Type, TypeList<ExistingTypes...>>
{
	using type = TypeList<ExistingTypes..., Type>;
};

// Zero-th case
template<>
struct ExternalAccessReverse<TypeList<>>
{
	using type = TypeList<>;
};

// N-th case
template<typename CurrentType, typename... RemainingTypes>
struct ExternalAccessReverse<TypeList<CurrentType, RemainingTypes...>>
{
	using RemainingTypeList = TypeList<RemainingTypes...>;
	using RemainingTypeListReversed = typename ExternalAccessReverse<RemainingTypeList>::type;
	using type = typename ExternalAccessAppend<CurrentType, RemainingTypeListReversed>::type;
};

// Zero-th case
template<>
struct ExternalAccessSplitKeepLatter<0, TypeList<>>
{
	using latter = TypeList<>;
};

// Zero-th case
template<typename CurrentType, typename... RemainingTypes>
struct ExternalAccessSplitKeepLatter<0, TypeList<CurrentType, RemainingTypes...>>
{
	using latter = TypeList<CurrentType, RemainingTypes...>;
};

// N-th case
template<size_t FirstIndexOfSecondSegment, typename CurrentType, typename... RemainingTypes>
struct ExternalAccessSplitKeepLatter<FirstIndexOfSecondSegment, TypeList<CurrentType, RemainingTypes...>>
{
	static_assert( FirstIndexOfSecondSegment - 1 <= sizeof...( RemainingTypes ), "Attempting to split past the end of type list" );
	using latter = typename ExternalAccessSplitKeepLatter<FirstIndexOfSecondSegment - 1, TypeList<RemainingTypes...>>::latter;
};

// Single case
template<size_t FirstIndexOfSecondSegment, typename TypeListType>
struct ExternalAccessSplit
{
private:
	static constexpr size_t kReversedLastIndexOfFirstSegment = TypeListType::kNumTypes - FirstIndexOfSecondSegment;
	using ReversedList = typename typelist_details::ExternalAccessReverse<TypeListType>::type;
	using SplitReversedList = typename typelist_details::ExternalAccessSplitKeepLatter<kReversedLastIndexOfFirstSegment, ReversedList>::latter;
public:
	using former = typename typelist_details::ExternalAccessReverse<SplitReversedList>::type;
	using latter = typename typelist_details::ExternalAccessSplitKeepLatter<FirstIndexOfSecondSegment, TypeListType>::latter;
};
}

///////////////////////////////////////////////////////////////////////////////
}
