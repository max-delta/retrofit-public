#include "stdafx.h"
#include "FramePackSerDes.h"

#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"

#include "PlatformFilesystem/VFS.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////
namespace fpackserdes_details {
static constexpr char kMagicType[4] = { 'F','P','A','K' };
static constexpr char kMagicVers[4] = { 'v','0','.','1' };
static constexpr char kMagic[8] = {
	kMagicType[0], kMagicType[1], kMagicType[2], kMagicType[3],
	kMagicVers[0], kMagicVers[1], kMagicVers[2], kMagicVers[3] };
}
///////////////////////////////////////////////////////////////////////////////

bool FramePackSerDes::SerializeToBuffer( gfx::TextureManager const & texMan, rftl::vector<uint8_t>& buffer, FramePackBase const & framePack )
{
	using namespace fpackserdes_details;

	static_assert( compiler::kEndianness == compiler::Endianness::Little, "This code is lazy, and doesn't support endianness changes" );

	using difference_type = rftl::remove_reference<decltype( buffer )>::type::difference_type;
	buffer.clear();

	constexpr size_t const kSizeOfMagic = sizeof( kMagic );
	constexpr size_t kSizeOfBase = sizeof( FramePackBase );
	size_t const sizeOfData_sustains = sizeof( uint8_t ) * framePack.m_MaxTimeSlots;
	size_t const sizeOfData_slots = sizeof( FramePackBase::TimeSlot ) * framePack.m_MaxTimeSlots;
	size_t const sizeOfData = sizeOfData_sustains + sizeOfData_slots;
	size_t const guessSizeOfPaths = ( sizeof( char ) * 128 ) *framePack.m_NumTimeSlots;

	size_t const guessSizeOfPayload = kSizeOfMagic + kSizeOfBase + sizeOfData + guessSizeOfPaths;
	buffer.reserve( guessSizeOfPayload );

	// Magic
	{
		buffer.resize( buffer.size() + kSizeOfMagic );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfMagic ) - 1 );
		memcpy( writeHead, kMagic, kSizeOfMagic );
	}

	// Base
	{
		buffer.resize( buffer.size() + kSizeOfBase );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfBase ) - 1 );
		static_assert( rftl::is_polymorphic<FramePackBase>::value == false, "Virtual table on framepack, unsafe serialization assumptions" );
		memcpy( writeHead, &framePack, kSizeOfBase );
	}

	// Data
	{
		// Sustains
		{
			buffer.resize( buffer.size() + sizeOfData_sustains );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_sustains ) - 1 );
			uint8_t const* const readHead = framePack.GetTimeSlotSustains();
			memcpy( writeHead, readHead, sizeOfData_sustains );
		}

		// Slots
		{
			buffer.resize( buffer.size() + sizeOfData_slots );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_slots ) - 1 );
			uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( framePack.GetTimeSlots() );
			memcpy( writeHead, readHead, sizeOfData_slots );
		}
	}

	// Paths
	{
		FramePackBase::TimeSlot const* const timeSlots = framePack.GetTimeSlots();
		for( size_t i = 0; i < framePack.m_NumTimeSlots; i++ )
		{
			ManagedTextureID const texID = timeSlots[i].m_TextureReference;
			TextureManager::ResourceName const resName = texMan.SearchForResourceNameByResourceID( texID );
			if( resName.empty() )
			{
				RFLOG_ERROR( nullptr, RFCAT_PPU, "Failed to find a resource name for resourced ID %llu", texID );
				buffer.clear();
				return false;
			}
			TextureManager::Filename const resFile = texMan.SearchForFilenameByResourceName( resName );
			if( resFile.Empty() )
			{
				RFLOG_ERROR( nullptr, RFCAT_PPU, "Failed to determine filename for resourced names \"%llu\"", resName.c_str() );
				buffer.clear();
				return false;
			}

			// NOTE: Pascal-style strings, since file paths can theoretically
			//  store some really wierd shit on some platforms, such as nulls
			std::string const fileStr = file::VFS::CreateStringFromPath( resFile );
			using StrLenType = uint16_t;
			StrLenType const fileStrLen = math::integer_cast<StrLenType>( fileStr.size() );

			// String length
			{
				buffer.resize( buffer.size() + sizeof( StrLenType ) );
				uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeof( StrLenType ) ) - 1 );
				uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( &fileStrLen );
				memcpy( writeHead, readHead, sizeof( StrLenType ) );
			}

			// String
			{
				buffer.resize( buffer.size() + fileStrLen );
				uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( fileStrLen ) - 1 );
				uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( fileStr.data() );
				memcpy( writeHead, readHead, fileStrLen );
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
