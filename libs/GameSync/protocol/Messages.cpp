#include "stdafx.h"
#include "Messages.h"


namespace RF::sync::protocol {
///////////////////////////////////////////////////////////////////////////////

void MsgHello::Append( Buffer& bytes ) const
{
	// No data
}



ReadResult MsgHello::TryRead( rftl::byte_view& bytes )
{
	// No data
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////

void MsgWelcome::Append( Buffer& bytes ) const
{
	// No data
}



ReadResult MsgWelcome::TryRead( rftl::byte_view& bytes )
{
	// No data
	return ReadResult::kSuccess;
}

///////////////////////////////////////////////////////////////////////////////
}
