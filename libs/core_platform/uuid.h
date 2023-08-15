#pragma once
#include "rftl/cstdint"
#include "rftl/string"


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

// A "Universally Unique IDentifer" to be used in distributed scenarios across
//  multiple physically-seperated computers. These are non-trivial to create,
//  not gauranteed to be cryptographically secure, not gauranteed to be serial,
//  not gauranteed to be inspectable, and not 'just a random number'.
// NOTE: Some platform implementations are so expensive as to perform context
//  switches in order to create a new UUID
// NOTE: Some platform implementations have endinaness quirks, RF uuid stores
//  in big-endian (network byte order)
// NOTE: Some version/variant combinations of UUIDs have poorer uniqueness
//  gaurantees than others, users will need to have a plan for how to deal with
//  collisions relevant to the version their implementation uses (That plan may
//  be to just hope it won't ever happen, but that's on you bro)
// ADVICE: Follow RFC4122 guidelines, don't use these unless you have to, don't
//  implement your own (and if you do, make it statndards-compliant; RFC4122
//  has detailed sample implementation code)
class Uuid
{
	//
	// Types and constants
public:
	using Octet = uint8_t;
	static constexpr size_t kNumOctets = 16;
	// NOTE: Avoiding endian-swapping behavior by using array
	using OctetSequence = Octet[kNumOctets];
	static constexpr OctetSequence kInvalid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	//
	// Public methods
public:
	Uuid();
	Uuid( OctetSequence const& sequence );
	Uuid( Uuid const& rhs ) = default;
	Uuid& operator=( Uuid const& ) = delete;

	bool operator==( Uuid const& rhs ) const;
	bool operator!=( Uuid const& rhs ) const;
	bool operator<( Uuid const& rhs ) const;
	bool operator>( Uuid const& rhs ) const;
	bool operator<=( Uuid const& rhs ) const;
	bool operator>=( Uuid const& rhs ) const;

	bool IsValid() const;
	bool HasClearlyDefinedCollisionAvoidance() const;
	bool ExposesComputerInformation() const;

	// NOTE: 'Version' here deoesn't meen 'revision', it means 'choice'
	uint8_t GetVersion() const;
	uint8_t GetVariant() const;

	OctetSequence const& GetSequence() const;
	rftl::string GetDebugString() const;

	// NOTE: Implementation-defined, requires additional linkage to a platform
	static Uuid GenerateNewUuid();


	//
	// Private data
private:
	OctetSequence const mOctetSequence;
};
static_assert( sizeof( Uuid ) == 16, "Unexpected size" );

///////////////////////////////////////////////////////////////////////////////
}
