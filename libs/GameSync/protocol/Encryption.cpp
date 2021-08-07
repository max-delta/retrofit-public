#include "stdafx.h"
#include "Encryption.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

bool PrepareEncryptionRequest( EncryptionState& encryption )
{
	switch( encryption.mPending )
	{
		case EncryptionMode::kUnencrypted:
			return true;
		case EncryptionMode::kMockInert:
			return true;
		default:
			RF_DBGFAIL();
			return false;
	}
}



bool PrepareEncryptionResponse( EncryptionState& encryption )
{
	switch( encryption.mPending )
	{
		case EncryptionMode::kUnencrypted:
			return true;
		case EncryptionMode::kMockInert:
			return true;
		default:
			RF_DBGFAIL();
			return false;
	}
}



void ApplyPendingEncryption( EncryptionState& encryption )
{
	encryption.mMode = encryption.mPending;
}



bool EncryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption )
{
	RF_ASSERT( begin != end );

	if( encryption.mMode == EncryptionMode::kUnencrypted )
	{
		return true;
	}
	else if( encryption.mMode == EncryptionMode::kMockInert )
	{
		// No change
		return true;
	}

	RF_DBGFAIL_MSG( "Unsupported encryption mode" );
	return false;
}



bool DecryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption )
{
	RF_ASSERT( begin != end );

	if( encryption.mMode == EncryptionMode::kUnencrypted )
	{
		return true;
	}
	else if( encryption.mMode == EncryptionMode::kMockInert )
	{
		// No change
		return true;
	}

	RF_DBGFAIL_MSG( "Unsupported encryption mode" );
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
