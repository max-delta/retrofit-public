#include "stdafx.h"
#include "FramePackSerDes.h"

#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PlatformFilesystem/VFS.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstring"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////
namespace fpackserdes_details {
static constexpr char kMagicType[4] = { 'F','P','A','K' };
static constexpr char kMagicVers[4] = { 'v','0','.','1' };
static constexpr char kMagic[8] = {
	kMagicType[0], kMagicType[1], kMagicType[2], kMagicType[3],
	kMagicVers[0], kMagicVers[1], kMagicVers[2], kMagicVers[3] };

using StrLenType = uint16_t;
constexpr size_t const kSizeOfMagic = sizeof( kMagic );
constexpr size_t kSizeOfBase = sizeof( FramePackBase );
constexpr size_t SizeOfData_Sustains( FramePackBase const & framePack )
{
	return sizeof( uint8_t ) * framePack.m_NumTimeSlots;
}
constexpr size_t SizeOfData_Slots( FramePackBase const & framePack )
{
	return sizeof( FramePackBase::TimeSlot ) * framePack.m_NumTimeSlots;
}

}
///////////////////////////////////////////////////////////////////////////////

bool FramePackSerDes::SerializeToBuffer( gfx::TextureManager const & texMan, rftl::vector<uint8_t>& buffer, FramePackBase const & framePack )
{
	using namespace fpackserdes_details;

	static_assert( compiler::kEndianness == compiler::Endianness::Little, "This code is lazy, and doesn't support endianness changes" );

	using difference_type = rftl::remove_reference<decltype( buffer )>::type::difference_type;
	nullptr_t const context = nullptr;
	buffer.clear();

	size_t const sizeOfData_sustains = SizeOfData_Sustains( framePack );
	size_t const sizeOfData_slots = SizeOfData_Slots( framePack );
	size_t const sizeOfData = sizeOfData_sustains + sizeOfData_slots;
	size_t const guessSizeOfPaths = ( sizeof( char ) * 128 ) * framePack.m_NumTimeSlots;

	size_t const guessSizeOfPayload = kSizeOfMagic + kSizeOfBase + sizeOfData + guessSizeOfPaths;
	buffer.reserve( guessSizeOfPayload );

	// Magic
	{
		buffer.resize( buffer.size() + kSizeOfMagic );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfMagic ) - 1 );
		rftl::memcpy( writeHead, kMagic, kSizeOfMagic );
	}

	// Base
	{
		buffer.resize( buffer.size() + kSizeOfBase );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfBase ) - 1 );
		static_assert( rftl::is_polymorphic<FramePackBase>::value == false, "Virtual table on framepack, unsafe serialization assumptions" );
		rftl::memcpy( writeHead, &framePack, kSizeOfBase );
	}

	// Data
	{
		// Sustains
		{
			buffer.resize( buffer.size() + sizeOfData_sustains );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_sustains ) - 1 );
			uint8_t const* const readHead = framePack.GetTimeSlotSustains();
			rftl::memcpy( writeHead, readHead, sizeOfData_sustains );
		}

		// Slots
		{
			buffer.resize( buffer.size() + sizeOfData_slots );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_slots ) - 1 );
			uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( framePack.GetTimeSlots() );
			rftl::memcpy( writeHead, readHead, sizeOfData_slots );
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
				RFLOG_ERROR( context, RFCAT_PPU, "Failed to find a resource name for resourced ID %llu", texID );
				buffer.clear();
				return false;
			}
			TextureManager::Filename const resFile = texMan.SearchForFilenameByResourceName( resName );
			if( resFile.Empty() )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Failed to determine filename for resourced names \"%llu\"", resName.c_str() );
				buffer.clear();
				return false;
			}

			// NOTE: Pascal-style strings, since file paths can theoretically
			//  store some really wierd shit on some platforms, such as nulls
			std::string const fileStr = file::VFS::CreateStringFromPath( resFile );
			StrLenType const fileStrLen = math::integer_cast<StrLenType>( fileStr.size() );

			// String length
			{
				buffer.resize( buffer.size() + sizeof( StrLenType ) );
				uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeof( StrLenType ) ) - 1 );
				uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( &fileStrLen );
				rftl::memcpy( writeHead, readHead, sizeof( StrLenType ) );
			}

			// String
			{
				buffer.resize( buffer.size() + fileStrLen );
				uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( fileStrLen ) - 1 );
				uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( fileStr.data() );
				rftl::memcpy( writeHead, readHead, fileStrLen );
			}
		}
	}

	return true;
}



bool FramePackSerDes::DeserializeFromBuffer( rftl::vector<file::VFSPath>& textures, rftl::vector<uint8_t> const & buffer, UniquePtr<FramePackBase>& framePack )
{
	using namespace fpackserdes_details;

	static_assert( compiler::kEndianness == compiler::Endianness::Little, "This code is lazy, and doesn't support endianness changes" );

	using difference_type = rftl::remove_reference<decltype( buffer )>::type::difference_type;
	nullptr_t const context = nullptr;
	uint8_t const* readHead = buffer.data();
	uint8_t const* const maxReadHead = readHead + buffer.size();
	framePack = nullptr;
	textures.clear();

	// Magic
	{
		if( readHead + kSizeOfMagic > maxReadHead )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read magic" );
			return false;
		}

		char magic[kSizeOfMagic];
		uint8_t* const writeHead = reinterpret_cast<uint8_t*>( &magic[0] );
		rftl::memcpy( writeHead, readHead, kSizeOfMagic );
		readHead += kSizeOfMagic;

		if( rftl::memcmp( &magic[0], kMagic, kSizeOfMagic ) != 0 )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Magic doesn't match" );
			return false;
		}
	}

	// Num slots
	size_t numSlots;
	{
		if( readHead + kSizeOfBase > maxReadHead )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read framepack base" );
			return false;
		}

		FramePackBase const& theoreticalBase = *reinterpret_cast<FramePackBase const*>( readHead );
		size_t const theoreticalSizeOfData_sustains = SizeOfData_Sustains( theoreticalBase );
		size_t const theoreticalSizeOfData_slots = SizeOfData_Slots( theoreticalBase );
		size_t const theoreticalSizeOfData = theoreticalSizeOfData_sustains + theoreticalSizeOfData_slots;
		size_t const maxSlots = theoreticalBase.m_MaxTimeSlots;
		numSlots = theoreticalBase.m_NumTimeSlots;

		if( maxSlots == 0 )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Zero max slots in framepack base, probably corrupt" );
			return false;
		}
		if( numSlots == 0 )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Zero slots in framepack base, probably corrupt" );
			return false;
		}

		if( maxSlots > theoreticalSizeOfData )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "More max slots in framepack base than could theoretically be in buffer, probably corrupt" );
			return false;
		}
		if( numSlots > theoreticalSizeOfData )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "More slots in framepack base than could theoretically be in buffer, probably corrupt" );
			return false;
		}

		if( numSlots > maxSlots )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "More slots than max slots framepack base, probably corrupt" );
			return false;
		}
	}

	// Allocate
	UniquePtr<FramePackBase> retValFPack;
	{
		if( numSlots <= gfx::FramePack_256::k_MaxTimeSlots )
		{
			retValFPack = DefaultCreator<gfx::FramePack_256>::Create();
		}
		else if( numSlots <= gfx::FramePack_512::k_MaxTimeSlots )
		{
			retValFPack = DefaultCreator<gfx::FramePack_512>::Create();
		}
		else if( numSlots <= gfx::FramePack_1024::k_MaxTimeSlots )
		{
			retValFPack = DefaultCreator<gfx::FramePack_1024>::Create();
		}
		else if( numSlots <= gfx::FramePack_4096::k_MaxTimeSlots )
		{
			retValFPack = DefaultCreator<gfx::FramePack_4096>::Create();
		}
		else
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Unable to determine allocation for frame pack" );
			return false;
		}
	}

	// Base
	FramePackBase& retBase = *retValFPack.Get();
	{
		// This should've been checked in slot calculations
		RF_ASSERT( readHead + kSizeOfBase <= maxReadHead );

		FramePackBase const& theoreticalBase = *reinterpret_cast<FramePackBase const*>( readHead );
		retBase.CopyBaseValuesFrom( theoreticalBase );
		readHead += kSizeOfBase;
	}

	// Data
	{
		// Sustains
		size_t const sizeOfData_sustains = SizeOfData_Sustains( retBase );
		{
			// This should've been checked in slot calculations
			RF_ASSERT( readHead + sizeOfData_sustains <= maxReadHead );

			uint8_t* const writeHead = reinterpret_cast<uint8_t*>( retBase.GetMutableTimeSlotSustains() );
			rftl::memcpy( writeHead, readHead, sizeOfData_sustains );
			readHead += sizeOfData_sustains;
		}

		// Slots
		size_t const sizeOfData_slots = SizeOfData_Slots( retBase );
		{
			// This should've been checked in slot calculations
			RF_ASSERT( readHead + sizeOfData_sustains <= maxReadHead );

			uint8_t* const writeHead = reinterpret_cast<uint8_t*>( retBase.GetMutableTimeSlots() );
			rftl::memcpy( writeHead, readHead, sizeOfData_slots );
			readHead += sizeOfData_slots;
		}
	}

	// Invalidation
	{
		FramePackBase::TimeSlot* const timeSlots = retBase.GetMutableTimeSlots();
		for( size_t i = 0; i < retBase.m_MaxTimeSlots; i++ )
		{
			FramePackBase::TimeSlot& timeSlot = timeSlots[i];
			timeSlot.m_TextureReference = k_InvalidManagedTextureID;
			timeSlot.m_ColliderReference = k_InvalidManagedColliderID;
		}
	}

	// Paths
	rftl::vector<file::VFSPath> retValTextures;
	{
		for( size_t i = 0; i < retBase.m_NumTimeSlots; i++ )
		{
			if( readHead + sizeof(StrLenType) > maxReadHead )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read length of file string, probably corrupt" );
				return false;
			}

			// String length
			StrLenType fileStrLen;
			{
				uint8_t* const writeHead = reinterpret_cast<uint8_t*>( &fileStrLen );
				rftl::memcpy( writeHead, readHead, sizeof( StrLenType ) );
				readHead += sizeof( StrLenType );
			}

			if( fileStrLen == 0 )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Zero bytes in file string, probably corrupt" );
				return false;
			}

			if( readHead + fileStrLen > maxReadHead )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read file string, probably corrupt" );
				return false;
			}

			// String
			rftl::string fileStr;
			{
				fileStr.resize( fileStrLen );
				uint8_t* const writeHead = reinterpret_cast<uint8_t*>( fileStr.data() );
				rftl::memcpy( writeHead, readHead, fileStrLen );
				readHead += fileStrLen;
			}

			file::VFSPath const path = file::VFS::CreatePathFromString( fileStr );
			retValTextures.emplace_back( path );
		}
	}

	framePack = rftl::move( retValFPack );
	textures = rftl::move( retValTextures );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
