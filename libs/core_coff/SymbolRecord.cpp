#include "stdafx.h"

#include "SymbolRecord.h"

#include "core_math/math_bytes.h"
#include "core_math/math_casts.h"

#include "rftl/extension/stream_operations.h"
#include "rftl/array"
#include "rftl/istream"


namespace RF::bin::coff {
///////////////////////////////////////////////////////////////////////////////

bool SymbolRecord::TryRead( rftl::streambuf& seekable, size_t seekBase )
{
	*this = {};

	rftl::istream stream( &seekable );
	bool const seekSuccess = rftl::stream_seek_abs( stream, seekBase );
	if( seekSuccess == false )
	{
		// Failed to seek
		return false;
	}

	static constexpr size_t kBytesPerRecord = 18;
	static constexpr size_t kBytesToRead = kBytesPerRecord;
	rftl::array<uint8_t, kBytesToRead> buffer;
	size_t const numRead = rftl::stream_read( buffer.data(), buffer.size(), stream );
	if( numRead != buffer.size() )
	{
		// Failed to read
		return false;
	}
	rftl::byte_view const bufferView( buffer.begin(), buffer.end() );
	rftl::byte_view readHead = bufferView;

	static constexpr size_t kNameLen = 8;
	static_assert( decltype( mName )::fixed_capacity == kNameLen );
	mName.resize( kNameLen, '?' );
	readHead.mem_copy_prefix_to( mName );
	readHead.remove_prefix( kNameLen );

	mValue = math::FromLittleEndianToPlatform( readHead.extract_front<uint32_t>() );
	mSectionNumber = math::FromLittleEndianToPlatform( readHead.extract_front<uint16_t>() );
	mTypeUpper = math::enum_bitcast<SymbolTypeUpper>( math::FromLittleEndianToPlatform( readHead.extract_front<uint8_t>() ) );
	mTypeLower = math::enum_bitcast<SymbolTypeLower>( math::FromLittleEndianToPlatform( readHead.extract_front<uint8_t>() ) );
	mStorageClass = math::enum_bitcast<SymbolStorageClass>( math::FromLittleEndianToPlatform( readHead.extract_front<uint8_t>() ) );
	mNumAuxSymbols = math::FromLittleEndianToPlatform( readHead.extract_front<uint8_t>() );
	RF_ASSERT( readHead.empty() );
	mRelativeOffsetToFirstAuxSymbol = kBytesToRead;
	mRelativeOffsetToNextSymbol = mRelativeOffsetToFirstAuxSymbol + kBytesPerRecord * mNumAuxSymbols;
	if( mName.empty() )
	{
		rftl::byte_view nameView( mName.data(), mName.fixed_capacity );
		uint32_t const zeros = nameView.extract_front<uint32_t>();
		if( zeros != 0 )
		{
			RF_DBGFAIL();
			return false;
		}
		mNameOffsetIntoStringTable = math::FromLittleEndianToPlatform( nameView.extract_front<uint32_t>() );
		RF_ASSERT( nameView.empty() );
	}

	return true;
}



rftl::string_view SymbolRecord::TryReadName( rftl::byte_view stringTable ) const
{
	if( mName.empty() == false )
	{
		RF_ASSERT( mNameOffsetIntoStringTable.has_value() == false );
		return mName;
	}

	RF_ASSERT( mNameOffsetIntoStringTable.has_value() );
	size_t const offset = mNameOffsetIntoStringTable.value();
	if( offset > stringTable.size() )
	{
		return {};
	}

	rftl::byte_view const maxBytes = stringTable.substr( offset );
	rftl::string_view const maxString( reinterpret_cast<char const*>( maxBytes.data() ), maxBytes.size() );
	size_t const firstNull = maxString.find( '\0', 0 );
	if( firstNull == rftl::string_view::npos )
	{
		// Even the last string should be null-terminated
		RF_DBGFAIL();
		return {};
	}
	rftl::string_view const singleString = maxString.substr( 0, firstNull );
	return singleString;
}

///////////////////////////////////////////////////////////////////////////////
}
