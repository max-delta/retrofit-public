#pragma once

#include <type_traits>
#include <stdint.h>

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

	// Used to search for the index of a type
	template<typename Type>
	struct FindIndex;


private:
	// For external access
	// NOTE: Not actually available externally, but the pattern illustrates how
	//  to build other logic for unpacking the type list
	template<size_t Index, typename TypeListType>
	struct ExternalAccessByIndex;
	template<typename Type, typename TypeListType>
	struct ExternalAccessContains;
	template<typename Type, size_t CurrentIndex, typename TypeListType>
	struct ExternalAccessFindIndex;

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

	// 0 case
	template<typename Type, int64_t CurrentIndex>
	struct ExternalAccessFindIndex<Type, CurrentIndex, TypeList<> >
	{
		static constexpr int64_t value = -1;
	};

	// N case
	template<typename Type, int64_t CurrentIndex, typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessFindIndex<Type, CurrentIndex, TypeList<CurrentType, RemainingTypes...> >
	{
		using IsCurrent = std::is_same< Type, CurrentType >;
		static constexpr int64_t value = IsCurrent::value ? CurrentIndex : ExternalAccessFindIndex<Type, CurrentIndex + 1, TypeList<RemainingTypes...> >::value;
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

	// Implemented as external
	template<typename Type>
	struct FindIndex : ExternalAccessFindIndex< Type, 0, TypeList<Types...> >
	{
		//
	};
};

///////////////////////////////////////////////////////////////////////////////
}
