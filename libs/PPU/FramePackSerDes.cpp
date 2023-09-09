#include "stdafx.h"
#include "FramePackSerDes.h"

#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"
#include "PlatformFilesystem/VFS.h"
#include "Allocation/AccessorDeclaration.h"

#include "core_allocate/DefaultAllocCreator.h"
#include "core_math/math_casts.h"

#include "core/ptr/default_creator.h"

#include "rftl/cstring"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////
namespace fpackserdes_details {
static constexpr char kMagicType[4] = { 'F', 'P', 'A', 'K' };
static constexpr char kMagicVers[4] = { 'v', '0', '.', '1' };
static constexpr char kMagic[8] = {
	kMagicType[0], kMagicType[1], kMagicType[2], kMagicType[3],
	kMagicVers[0], kMagicVers[1], kMagicVers[2], kMagicVers[3] };

using StrLenType = uint16_t;
constexpr size_t const kSizeOfMagic = sizeof( kMagic );
constexpr size_t kSizeOfBase = sizeof( FramePackBase );
constexpr size_t SizeOfData_Sustains( FramePackBase const& framePack )
{
	return sizeof( uint8_t ) * framePack.mNumTimeSlots;
}
constexpr size_t SizeOfData_Slots( FramePackBase const& framePack )
{
	return sizeof( FramePackBase::TimeSlot ) * framePack.mNumTimeSlots;
}

struct Header_v0_1
{
	uint16_t mOffsetToMagic;
	uint16_t mOffsetToHeader;
	uint16_t mOffsetToBase;
	uint16_t mOffsetToSustains;
	uint16_t mOffsetToSlots;
	uint16_t mOffsetToTexturePaths;
	uint16_t mOffsetToEof;

	bool IsValid()
	{
		nullptr_t const context = nullptr;
		if( mOffsetToMagic != 0 )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Invalid header, magic offset is wrong" );
			return false;
		}
		if( mOffsetToHeader != mOffsetToMagic + kSizeOfMagic )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Invalid header, header offset is wrong" );
			return false;
		}
		if( mOffsetToBase != mOffsetToHeader + sizeof( *this ) )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Invalid header, base offset is wrong" );
			return false;
		}
		if( mOffsetToSustains != mOffsetToBase + kSizeOfBase )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Invalid header, sustains offset is wrong" );
			return false;
		}
		return true;
	}
};
static_assert( sizeof( Header_v0_1 ) == 14, "Unexpected size" );

}
///////////////////////////////////////////////////////////////////////////////

bool FramePackSerDes::SerializeToBuffer( gfx::TextureManager const& texMan, rftl::vector<uint8_t>& buffer, FramePackBase const& framePack )
{
	using namespace fpackserdes_details;

	static_assert( compiler::kEndianness == compiler::Endianness::Little, "This code is lazy, and doesn't support endianness changes" );

	using CurrentHeaderVersion = Header_v0_1;
	using difference_type = rftl::remove_reference<decltype( buffer )>::type::difference_type;
	nullptr_t const context = nullptr;
	buffer.clear();

	// Collect texture path data
	rftl::vector<rftl::string> texturePaths;
	size_t sizeOfTexturePaths = 0;
	texturePaths.reserve( framePack.mNumTimeSlots );
	{
		FramePackBase::TimeSlot const* const timeSlots = framePack.GetTimeSlots();
		for( size_t i = 0; i < framePack.mNumTimeSlots; i++ )
		{
			ManagedTextureID const texID = timeSlots[i].mTextureReference;
			TextureManager::ResourceName const resName = texMan.SearchForResourceNameByResourceID( texID );
			if( resName.empty() )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Failed to find a resource name for resourced ID %llu", texID );
				return false;
			}
			TextureManager::Filename const resFile = texMan.SearchForFilenameByResourceName( resName );
			if( resFile.Empty() )
			{
				RFLOG_ERROR( context, RFCAT_PPU, "Failed to determine filename for resource name \"%s\"", resName.c_str() );
				return false;
			}

			rftl::string fileStr = resFile.CreateString();

			// NOTE: Pascal strings
			sizeOfTexturePaths += sizeof( StrLenType );
			sizeOfTexturePaths += fileStr.size();

			texturePaths.emplace_back( rftl::move( fileStr ) );
		}
	}

	// Prepare header
	CurrentHeaderVersion header{};
	header.mOffsetToMagic = 0;
	header.mOffsetToHeader = math::integer_cast<uint16_t>( header.mOffsetToMagic + kSizeOfMagic );
	size_t const sizeOfHeader = sizeof( CurrentHeaderVersion );
	header.mOffsetToBase = math::integer_cast<uint16_t>( header.mOffsetToHeader + sizeOfHeader );
	header.mOffsetToSustains = math::integer_cast<uint16_t>( header.mOffsetToBase + kSizeOfBase );
	size_t const sizeOfData_sustains = SizeOfData_Sustains( framePack );
	header.mOffsetToSlots = math::integer_cast<uint16_t>( header.mOffsetToSustains + sizeOfData_sustains );
	size_t const sizeOfData_slots = SizeOfData_Slots( framePack );
	header.mOffsetToTexturePaths = math::integer_cast<uint16_t>( header.mOffsetToSlots + sizeOfData_slots );
	header.mOffsetToEof = math::integer_cast<uint16_t>( header.mOffsetToTexturePaths + sizeOfTexturePaths );

	buffer.reserve( header.mOffsetToEof );

	// Magic
	{
		buffer.resize( buffer.size() + kSizeOfMagic );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfMagic ) - 1 );
		RF_ASSERT( writeHead == buffer.data() + header.mOffsetToMagic );
		uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( kMagic );
		rftl::memcpy( writeHead, readHead, kSizeOfMagic );
	}

	// Header
	{
		buffer.resize( buffer.size() + sizeOfHeader );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfHeader ) - 1 );
		RF_ASSERT( writeHead == buffer.data() + header.mOffsetToHeader );
		uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( &header );
		rftl::memcpy( writeHead, readHead, sizeOfHeader );
	}

	// Base
	{
		buffer.resize( buffer.size() + kSizeOfBase );
		uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( kSizeOfBase ) - 1 );
		RF_ASSERT( writeHead == buffer.data() + header.mOffsetToBase );
		uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( &framePack );
		static_assert( rftl::is_polymorphic<FramePackBase>::value == false, "Virtual table on framepack, unsafe serialization assumptions" );
		rftl::memcpy( writeHead, readHead, kSizeOfBase );
	}

	// Data
	{
		// Sustains
		{
			buffer.resize( buffer.size() + sizeOfData_sustains );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_sustains ) - 1 );
			RF_ASSERT( writeHead == buffer.data() + header.mOffsetToSustains );
			uint8_t const* const readHead = framePack.GetTimeSlotSustains();
			rftl::memcpy( writeHead, readHead, sizeOfData_sustains );
		}

		// Slots
		{
			buffer.resize( buffer.size() + sizeOfData_slots );
			uint8_t* const writeHead = &*( buffer.rbegin() + math::integer_cast<difference_type>( sizeOfData_slots ) - 1 );
			RF_ASSERT( writeHead == buffer.data() + header.mOffsetToSlots );
			uint8_t const* const readHead = reinterpret_cast<uint8_t const*>( framePack.GetTimeSlots() );
			rftl::memcpy( writeHead, readHead, sizeOfData_slots );
		}
	}

	// Paths
	{
		RF_ASSERT( buffer.size() == header.mOffsetToTexturePaths );
		for( size_t i = 0; i < framePack.mNumTimeSlots; i++ )
		{
			// NOTE: Pascal-style strings, since file paths can theoretically
			//  store some really wierd shit on some platforms, such as nulls
			rftl::string const& fileStr = texturePaths.at( i );
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

	RF_ASSERT( buffer.size() == header.mOffsetToEof );
	return true;
}



bool FramePackSerDes::DeserializeFromBuffer( rftl::vector<file::VFSPath>& textures, rftl::vector<uint8_t> const& buffer, UniquePtr<FramePackBase>& framePack )
{
	using namespace fpackserdes_details;

	static_assert( compiler::kEndianness == compiler::Endianness::Little, "This code is lazy, and doesn't support endianness changes" );

	using CurrentHeaderVersion = Header_v0_1;
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

	// Header
	CurrentHeaderVersion header;
	size_t const sizeOfHeader = sizeof( CurrentHeaderVersion );
	{
		if( readHead + kSizeOfMagic > maxReadHead )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read header, probably corrupt" );
			return false;
		}

		uint8_t* const writeHead = reinterpret_cast<uint8_t*>( &header );
		rftl::memcpy( writeHead, readHead, sizeOfHeader );
		readHead += sizeOfHeader;

		if( header.IsValid() == false )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Invalid header, probably corrupt" );
			return false;
		}
	}

	// Num slots
	size_t numSlots;
	{
		if( readHead + kSizeOfBase > maxReadHead )
		{
			RFLOG_ERROR( context, RFCAT_PPU, "Not enough bytes to read framepack base, probably corrupt" );
			return false;
		}

		FramePackBase const& theoreticalBase = *reinterpret_cast<FramePackBase const*>( readHead );
		size_t const theoreticalSizeOfData_sustains = SizeOfData_Sustains( theoreticalBase );
		size_t const theoreticalSizeOfData_slots = SizeOfData_Slots( theoreticalBase );
		size_t const theoreticalSizeOfData = theoreticalSizeOfData_sustains + theoreticalSizeOfData_slots;
		size_t const maxSlots = theoreticalBase.mMaxTimeSlots;
		numSlots = theoreticalBase.mNumTimeSlots;

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
		if( numSlots <= FramePack_256::kMaxTimeSlots )
		{
			retValFPack = alloc::DefaultAllocCreator<FramePack_256>::Create( *alloc::GetAllocator<RFTAG_PPU>() );
		}
		else if( numSlots <= FramePack_512::kMaxTimeSlots )
		{
			retValFPack = alloc::DefaultAllocCreator<FramePack_512>::Create( *alloc::GetAllocator<RFTAG_PPU>() );
		}
		else if( numSlots <= FramePack_1024::kMaxTimeSlots )
		{
			retValFPack = alloc::DefaultAllocCreator<FramePack_1024>::Create( *alloc::GetAllocator<RFTAG_PPU>() );
		}
		else if( numSlots <= FramePack_4096::kMaxTimeSlots )
		{
			retValFPack = alloc::DefaultAllocCreator<FramePack_4096>::Create( *alloc::GetAllocator<RFTAG_PPU>() );
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
		for( size_t i = 0; i < retBase.mMaxTimeSlots; i++ )
		{
			FramePackBase::TimeSlot& timeSlot = timeSlots[i];
			timeSlot.mTextureReference = kInvalidManagedTextureID;
			timeSlot.mColliderReference = kInvalidManagedColliderID;
		}
	}

	// Paths
	rftl::vector<file::VFSPath> retValTextures;
	{
		for( size_t i = 0; i < retBase.mNumTimeSlots; i++ )
		{
			if( readHead + sizeof( StrLenType ) > maxReadHead )
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

			file::VFSPath const path = file::VFSPath::CreatePathFromString( fileStr );
			retValTextures.emplace_back( path );
		}
	}

	framePack = rftl::move( retValFPack );
	textures = rftl::move( retValTextures );
	return true;
}

///////////////////////////////////////////////////////////////////////////////
}
