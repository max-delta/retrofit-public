#pragma once
#include "LoggingRouter.h"

#include <cstdarg>


namespace RF { namespace logging {
///////////////////////////////////////////////////////////////////////////////

template<typename Context>
inline void LoggingRouter::Log( Context context, CategoryKey categoryKey, SeverityMask severityMask, char const * format, ... ) const
{
	LogContextBuffer contextBuffer = LogContextBuffer();
	WriteContextString( context, contextBuffer );
	*contextBuffer.rend() = '\0';

	va_list args;
	va_start( args, format );
	LogInternal( &contextBuffer[0], categoryKey, severityMask, format, args );
	va_end( args );
}

///////////////////////////////////////////////////////////////////////////////
}}
