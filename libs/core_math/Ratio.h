#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF::math {
///////////////////////////////////////////////////////////////////////////////

// A positive fractional type that tries to be clever about expressing ratios
//  is a lossless and performant way
template<typename StorageT, typename InterfaceT>
class Ratio
{
	//
	// Types
public:
	using StorageType = StorageT;
	using InterfaceType = InterfaceT;
	using Pair = rftl::pair<InterfaceType, InterfaceType>;

	// NOTE: This also means we can just copy around instead of taking
	//  references, for minor speed gains
	static_assert( rftl::is_integral<InterfaceType>::value, "Only primitive integral types supported" );

	// This simplifies logic, saves up precious bits, and further enforces
	//  these as 'ratios' and not just 'fractions'
	static_assert( rftl::is_unsigned<InterfaceType>::value, "Only positive integral types supported" );


	//
	// Public methods
public:
	Ratio();
	Ratio( InterfaceType numerator, InterfaceType denominator );
	explicit Ratio( Pair pair );

	// Ratios can implicitly up-size to a larger storage as needed
	template<typename OtherRatioT, rftl::enable_if<sizeof( typename OtherRatioT::StorageType ) < sizeof( StorageType ), int>::type = 0>
	Ratio( OtherRatioT rhs );

	// Accessors for underlying data
	Pair GetAsPair() const;
	float GetAsFloat() const;
	InterfaceType Numerator() const;
	InterfaceType Denominator() const;

	// The validity is simply whether the denominator is non-zero, as division
	//  by zero is the only invalid value
	bool IsValid() const;

	// If true, it can be cleanly expressed as a non-fractional number
	bool IsIntegral() const;

	// Simplifies a fraction, such as 4/6 -> 2/3
	// NOTE: Useful for helping keep the components within their storage range
	Ratio Simplify() const;

	// Compare with integer
	// NOTE: For simplicity, invalid numbers (zero as denominator) are treated
	//  as though they are zero
	bool operator<( InterfaceT rhs ) const;
	bool operator<=( InterfaceT rhs ) const;
	bool operator==( InterfaceT rhs ) const;
	bool operator>=( InterfaceT rhs ) const;
	bool operator>( InterfaceT rhs ) const;

	// Compare with other ratio
	// NOTE: For simplicity, invalid numbers (zero as denominator) are treated
	//  as though they are zero
	bool operator<( Ratio rhs ) const;
	bool operator<=( Ratio rhs ) const;
	bool operator==( Ratio rhs ) const;
	bool operator>=( Ratio rhs ) const;
	bool operator>( Ratio rhs ) const;

	// Standard math
	// NOTE: Overflows will result in the ratio being set to invalid
	Ratio operator+( Ratio rhs ) const;
	Ratio operator-( Ratio rhs ) const;
	Ratio operator*( Ratio rhs ) const;
	Ratio operator/( Ratio rhs ) const;

	// In-place math
	// NOTE: Overflows will result in the ratio being set to invalid
	Ratio& operator+=( Ratio rhs );
	Ratio& operator-=( Ratio rhs );
	Ratio& operator*=( Ratio rhs );
	Ratio& operator/=( Ratio rhs );


	//
	// Private data
private:
	StorageType mStorage = {};
};

///////////////////////////////////////////////////////////////////////////////

template<typename StorageT, typename InterfaceT>
template<typename OtherRatioT, rftl::enable_if<sizeof( typename OtherRatioT::StorageType ) < sizeof( StorageT ), int>::type>
inline Ratio<StorageT, InterfaceT>::Ratio( OtherRatioT rhs )
	: Ratio( rhs.Numerator(), rhs.Denominator() )
{
	//
}

///////////////////////////////////////////////////////////////////////////////
}

// Explicitly instantiate and alias common types
namespace RF::math {
using Ratio8 = Ratio<uint8_t, uint8_t>;
using Ratio16 = Ratio<uint16_t, uint8_t>;
using Ratio32 = Ratio<uint32_t, uint16_t>;
}
extern template class RF::math::Ratio<uint8_t, uint8_t>;
extern template class RF::math::Ratio<uint16_t, uint8_t>;
extern template class RF::math::Ratio<uint32_t, uint16_t>;
