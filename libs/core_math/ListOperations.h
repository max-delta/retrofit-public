#pragma once
#include "core/meta/ValueList.h"
#include "core_math/NumericTraits.h"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

template<typename ValueType, typename ValueListType>
struct ListSum;

template<typename ValueType>
struct ListSum<ValueType, ValueList<ValueType>>
{
	static constexpr ValueType value = NumericTraits<ValueType>::Empty();
};

template<typename ValueType, ValueType CurrentValue, ValueType... RemainingValues>
struct ListSum<ValueType, ValueList<ValueType, CurrentValue, RemainingValues...>>
{
	using RemainingValuesList = ValueList<ValueType, RemainingValues...>;
	static constexpr ValueType kSumOfRemainingValues = ListSum<ValueType, RemainingValuesList>::value;
	static constexpr ValueType value = CurrentValue + kSumOfRemainingValues;
};

///////////////////////////////////////////////////////////////////////////////

template<typename ValueType, typename ValueListType>
struct ListProduct;

template<typename ValueType>
struct ListProduct<ValueType, ValueList<ValueType>>
{
	static constexpr ValueType value = NumericTraits<ValueType>::Identity();
};

template<typename ValueType, ValueType CurrentValue, ValueType... RemainingValues>
struct ListProduct<ValueType, ValueList<ValueType, CurrentValue, RemainingValues...>>
{
	using RemainingValuesList = ValueList<ValueType, RemainingValues...>;
	static constexpr ValueType kProductOfRemainingValues = ListProduct<ValueType, RemainingValuesList>::value;
	static constexpr ValueType value = CurrentValue * kProductOfRemainingValues;
};

///////////////////////////////////////////////////////////////////////////////
}
