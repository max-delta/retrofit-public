#include "stdafx.h"
#include "project.h"

#include "Logging/Logging.h"

#include "core_platform/uuid.h"
#include "core_platform/inc/rpc_inc.h"
#include "core_math/math_bytes.h"
#include "core/compiler.h"
#include "core/macros.h"

#include "rftl/cstring"

#pragma comment( lib, "rpcrt4.lib" )


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

Uuid Uuid::GenerateNewUuid()
{
	win32::UUID winUuid{};
	win32::RPC_STATUS result;

	// NOTE: You can change this at will, safely, since the version and variant
	//  information encoded in the UUIDs means the two implementations can't
	//  ever collide
	enum class GenerationMode : uint8_t
	{
		// Exposes computer information, but collisions require same time and
		//  hardware identifiers
		Reliable,

		// RNG and pray
		Secure
	};
	constexpr GenerationMode kGenerationMode = GenerationMode::Secure;

	switch( kGenerationMode )
	{
		case GenerationMode::Reliable:
		{
			result = win32::UuidCreateSequential( &winUuid );
			break;
		}
		case GenerationMode::Secure:
		default:
		{
			result = win32::UuidCreate( &winUuid );
			break;
		}
	}

	switch( result )
	{
		case RPC_S_OK:
			break;
		case RPC_S_UUID_LOCAL_ONLY: // Something went wrong, not unique
		case RPC_S_UUID_NO_ADDRESS: // No ethernet information to make unique
		default: // Unknown failure
			return Uuid();
	}

	// Standard seems to gloss over byte-order, and assumes everyone has their
	//  shit together. This kinda assumes that people will make their pipes
	//  UUID-aware all the way through, which is a dangerous assumption.
	//  RetroFit treats UUIDs as bricks of indivisible data, and keeps them in
	//  network-byte-order all the way through so they can be mem-copied around
	//  without too much worry.
	// SEE:
	//  https://tools.ietf.org/html/rfc4122
	//  https://msdn.microsoft.com/en-us/library/ff718266.aspx
	//  https://msdn.microsoft.com/en-us/library/windows/desktop/aa379358(v=vs.85).aspx
	//  https://msdn.microsoft.com/en-us/library/windows/desktop/aa379205(v=vs.85).aspx
	//  https://blogs.msdn.microsoft.com/openspecification/2013/10/08/guids-and-endianness-endi-an-ne-ssinguid-or-idne-na-en-ssinguid/
	static_assert( compiler::kEndianness == compiler::Endianness::Little, "Check endianness assumptions" );
	static_assert( sizeof( win32::UUID ) == sizeof( Uuid::OctetSequence ), "Unexpected size" );
	using Data1 = decltype( winUuid.Data1 );
	using Data2 = decltype( winUuid.Data2 );
	using Data3 = decltype( winUuid.Data3 );
	using Data4 = decltype( winUuid.Data4 );
	static_assert( sizeof( Data1 ) + sizeof( Data2 ) + sizeof( Data3 ) + sizeof( Data4 ) == sizeof( Uuid::OctetSequence ), "Unexpected size" );
	Uuid::OctetSequence seq{};
	uint8_t* const seqStart = reinterpret_cast<uint8_t*>( seq );
	Data1* const seqData1 = reinterpret_cast<Data1*>( seqStart );
	Data2* const seqData2 = reinterpret_cast<Data2*>( seqData1 + 1 );
	Data3* const seqData3 = reinterpret_cast<Data3*>( seqData2 + 1 );
	Data4* const seqData4 = reinterpret_cast<Data4*>( seqData3 + 1 );
	static_assert( sizeof( Data1 ) == sizeof( uint32_t ), "Unepxected size" );
	static_assert( sizeof( Data2 ) == sizeof( uint16_t ), "Unepxected size" );
	static_assert( sizeof( Data3 ) == sizeof( uint16_t ), "Unepxected size" );
	*seqData1 = math::ReverseByteOrder( static_cast<uint32_t>( winUuid.Data1 ) );
	*seqData2 = math::ReverseByteOrder( static_cast<uint16_t>( winUuid.Data2 ) );
	*seqData3 = math::ReverseByteOrder( static_cast<uint16_t>( winUuid.Data3 ) );
	rftl::memcpy( seqData4, winUuid.Data4, sizeof( Data4 ) );
	Uuid const retVal{ seq };
	if( kGenerationMode == GenerationMode::Secure )
	{
		// This shouldn't fail if UuidCreateSequential(...) is present during
		//  compilation, since that function means this was compiled with a
		//  Windows SDK that switched UuidCreate(...) to be secure by default
		if( retVal.ExposesComputerInformation() )
		{
			RFLOG_CRITICAL( nullptr, RFCAT_PLATFORMUTILS, "Windows seems to have produced an unsecure UUID" );
			return Uuid();
		}
	}
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
