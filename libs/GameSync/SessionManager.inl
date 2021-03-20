#include "SessionManager.h"

namespace RF::sync {
///////////////////////////////////////////////////////////////////////////////

template<typename Message>
inline void SessionManager::AddBlindReader()
{
	WriterLock const lock( mBlindReadersMutex );

	mBlindReaders[Message::kID] =
		[]( rftl::byte_view& bytes ) -> protocol::ReadResult //
	{
		Message msg = {};
		return msg.TryRead( bytes );
	};
}

///////////////////////////////////////////////////////////////////////////////
}
