#pragma once

#include <type_traits>

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename... Types>
struct TypeList
{
	static constexpr size_t kNumTypes = sizeof...( Types );

	// Used to access types by index
	template<size_t Index>
	struct ByIndex;

	// Used to test for presence of a type
	template<typename Type>
	struct Contains;

private:
	// For external access
	// NOTE: Not actually available externally, but the pattern illustrates how
	//  to build other logic for unpacking the type list
	template<size_t Index, typename TypeListType>
	struct ExternalAccessByIndex;
	template<typename Type, typename TypeListType>
	struct ExternalAccessContains;

	// Zero-th case
	template<typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessByIndex<0, TypeList<CurrentType, RemainingTypes...> >
	{
		using type = CurrentType;
	};

	// N-th case
	template<size_t Index, typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessByIndex<Index, TypeList<CurrentType, RemainingTypes...> >
	{
		static_assert( Index - 1 < sizeof...( RemainingTypes ), "Attempting to index past the end of type list" );
		using type = typename ExternalAccessByIndex< Index - 1, TypeList<RemainingTypes...> >::type;
	};

	// 0 case
	template<typename Type>
	struct ExternalAccessContains<Type, TypeList<> >
	{
		static constexpr bool value = false;
	};

	// N case
	template<typename Type, typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessContains<Type, TypeList<CurrentType, RemainingTypes...> >
	{
		using IsCurrent = std::is_same< Type, CurrentType >;
		static constexpr bool value = std::disjunction< IsCurrent, ExternalAccessContains<Type, TypeList<RemainingTypes...> > >::value;
	};

public:
	// Implemented as external
	template<size_t Index>
	struct ByIndex : ExternalAccessByIndex < Index, TypeList<Types...>>
	{
		//
	};

	// Implemented as external
	template<typename Type>
	struct Contains : ExternalAccessContains< Type, TypeList<Types...> >
	{
		//
	};
};

///////////////////////////////////////////////////////////////////////////////
}
