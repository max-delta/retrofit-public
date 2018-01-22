#pragma once

namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename... Types>
struct TypeList
{
	static constexpr size_t kNumTypes = sizeof...( Types );

	// Used to access types by index
	template<size_t Index>
	struct ByIndex;

private:
	// For external access
	// NOTE: Not actually available externally, but the pattern illustrates how
	//  to build other logic for unpacking the type list
	template<size_t Index, typename TypeListType>
	struct ExternalAccessByIndex;

	// Zero-th case
	template<typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessByIndex<0, TypeList<CurrentType, RemainingTypes...> >
	{
		using Type = CurrentType;
	};

	// N-th case
	template<size_t Index, typename CurrentType, typename... RemainingTypes>
	struct ExternalAccessByIndex<Index, TypeList<CurrentType, RemainingTypes...> >
	{
		static_assert( Index - 1 < sizeof...( RemainingTypes ), "Attempting to index past the end of type list" );
		using Type = typename ExternalAccessByIndex< Index - 1, TypeList<RemainingTypes...> >::Type;
	};

	// Implemented as external
	template<size_t Index>
	struct ByIndex : ExternalAccessByIndex < Index, TypeList<Types...>>
	{
		//
	};
};

///////////////////////////////////////////////////////////////////////////////
}
