#include "stdafx.h"
#include "Encryption.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

bool EncryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption )
{
	if( encryption.mMode == EncryptionMode::kUnencrypted )
	{
		return true;
	}

	RF_TODO_BREAK();
	return false;
}



bool DecryptBytes( Buffer::iterator begin, Buffer::iterator end, EncryptionState const& encryption )
{
	if( encryption.mMode == EncryptionMode::kUnencrypted )
	{
		return true;
	}

	RF_TODO_BREAK();
	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
