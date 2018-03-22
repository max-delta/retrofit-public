#pragma once
#include "core/meta/ValueList.h"


namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

template<typename AccessTypeT, size_t... fieldSizes>
class BitField
{
public:
	using AccessType = AccessTypeT;
	using FieldSizes = ValueList<size_t, fieldSizes...>;
	static constexpr size_t kNumFields = FieldSizes::kNumValues;


public:
	AccessType Read( size_t index ) const;
	void Write( size_t index, AccessType const& value );


private:
	template<size_t destinationSizeBits, size_t destinationOffsetBits>
	void Compress( AccessType const& source, void* destination );

	template<size_t sourceSizeBits, size_t sourceOffsetBits>
	AccessType Decompress( void* source );
};

///////////////////////////////////////////////////////////////////////////////
}}
