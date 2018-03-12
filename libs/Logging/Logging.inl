#pragma once
#include "Logging.h"


namespace RF { namespace logging { namespace details {
///////////////////////////////////////////////////////////////////////////////

template<typename Context>
void Log( Context context, char const* categoryKey, uint64_t severityMask, char const* format, ... )
{
	LogContextBuffer contextBuffer = LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rbegin() = '\0';

	va_list args;
	va_start( args, format );
	details::LogVA( &contextBuffer[0], categoryKey, severityMask, format, args );
	va_end( args );
}

///////////////////////////////////////////////////////////////////////////////
}}}
