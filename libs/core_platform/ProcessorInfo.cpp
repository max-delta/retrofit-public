#include "stdafx.h"
#include "ProcessorInfo.h"

#include "core_math/BitField.h"

#include "core/compiler.h"
#include "core/rf_assert.h"

#include <rftl/array>
#include <rftl/string_view>
#include <rftl/extension/string_format.h>


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////
namespace details {

#if defined( RF_PLATFORM_X86_64 )
struct Registers
{
	uint32_t eax, ebx, ecx, edx = 0;
};
static_assert( sizeof( Registers ) == 4 * 4 );

static void CpuID( Registers& registers, uint32_t eax, uint32_t ecx )
{
	// EAX, EBX, ECX, EDX
	rftl::array<uint32_t, 4> temp = {};

	#if defined( RF_PLATFORM_MSVC ) || defined( RF_PLATFORM_CLANG )
	static_assert( sizeof( uint32_t ) == sizeof( int32_t ) );
	__cpuidex(
		reinterpret_cast<int32_t*>( temp.data() ),
		static_cast<int32_t>( eax ),
		static_cast<int32_t>( ecx ) );
	#endif

	registers = {
		temp[0],
		temp[1],
		temp[2],
		temp[3] };
}
#endif



#if defined( RF_PLATFORM_X86_64 )
static ProcessorInfo GenerateX86()
{
	ProcessorInfo retVal = {};

	Registers registers = {};

	// CPUID Fn0000_0000_EAX Largest Standard Function Number
	// CPUID Fn0000_0000_E[D,C,B]X Processor Vendor
	CpuID( registers, 0, 0 );
	{
		size_t const largestFunction = registers.eax;
		( (void)largestFunction );

		static_assert( sizeof( registers.ebx ) == 4 );
		rftl::string_view const b{ reinterpret_cast<char const*>( &registers.ebx ), 4 };
		rftl::string_view const c{ reinterpret_cast<char const*>( &registers.ecx ), 4 };
		rftl::string_view const d{ reinterpret_cast<char const*>( &registers.edx ), 4 };

		rftl::array<char, 12> vendorString = {};
		vendorString[0] = b[0];
		vendorString[1] = b[1];
		vendorString[2] = b[2];
		vendorString[3] = b[3];
		vendorString[4] = d[0];
		vendorString[5] = d[1];
		vendorString[6] = d[2];
		vendorString[7] = d[3];
		vendorString[8] = c[0];
		vendorString[9] = c[1];
		vendorString[10] = c[2];
		vendorString[11] = c[3];
		rftl::string_view const combined{ vendorString.data(), vendorString.size() };
		retVal.mVendorID = combined;
	}

	// CPUID Fn0000_0001_EAX Family, Model, Stepping Identifiers
	// CPUID Fn0000_0001_EBX LocalApicId, LogicalProcessorCount, CLFlush
	// CPUID Fn0000_0001_ECX Feature Identifiers
	// CPUID Fn0000_0001_EDX Feature Identifiers
	CpuID( registers, 1, 0 );
	{
		// 0: 4 Reserved
		// 1: 8 ExtFamily
		// 2: 4 ExtModel
		// 3: 4 Reserved
		// 4: 4 BaseFamily
		// 5: 4 BaseModel
		// 6: 4 Stepping
		static_assert( compiler::kEndianness == compiler::Endianness::Little );
		uint32_t const eax = math::ReverseByteOrder( registers.eax );
		using Identifier = math::BitField<4, 8, 4, 4, 4, 4, 4>;
		static_assert( sizeof( Identifier ) == sizeof( eax ) );
		Identifier const* const asIdentifier = reinterpret_cast<Identifier const*>( &eax );
		uint8_t const extFamily = asIdentifier->ReadAt<1, uint8_t>();
		uint8_t const baseFamily = asIdentifier->ReadAt<4, uint8_t>();
		uint8_t const family = static_cast<uint8_t>( extFamily + baseFamily );
		uint8_t const extModel = asIdentifier->ReadAt<2, uint8_t>();
		uint8_t const baseModel = asIdentifier->ReadAt<5, uint8_t>();
		uint8_t const model = static_cast<uint8_t>( ( extModel << 4 ) + baseModel );
		uint8_t const stepping = asIdentifier->ReadAt<6, uint8_t>();
		retVal.mModelID.clear();
		retVal.mModelID.reserve( 2 + 2 + 1 + ( 1 * 2 ) ); // "AA:BB:C"
		{
			char ch1, ch2 = 0;
			rftl::format_byte( ch1, ch2, family );
			retVal.mModelID.push_back( ch1 );
			retVal.mModelID.push_back( ch2 );
			retVal.mModelID.push_back( ':' );
			rftl::format_byte( ch1, ch2, model );
			retVal.mModelID.push_back( ch1 );
			retVal.mModelID.push_back( ch2 );
			retVal.mModelID.push_back( ':' );
			retVal.mModelID.push_back( rftl::format_nibble( stepping ) );
		}
	}

	return retVal;
}
#endif

}
///////////////////////////////////////////////////////////////////////////////

ProcessorInfo ProcessorInfo::Generate()
{
#if defined( RF_PLATFORM_X86_64 )
	{
		return details::GenerateX86();
	}
#else
	{
		ProcessorInfo retVal = {};
		retVal.mVendorID = "UNKNOWN";
		retVal.mModelID = "UNKNOWN";
		return retVal;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
}
