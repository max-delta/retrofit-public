#include "stdafx.h"

#include "LoggingRouter.h"

#include "core_math/math_bits.h"
#include "core_unicode/StringConvert.h"
#include "rftl/cstdarg"


namespace RF::logging {
///////////////////////////////////////////////////////////////////////////////
namespace details {

template<typename FromT, typename ToT>
class LogEventConverter
{
	RF_NO_COPY( LogEventConverter );

public:
	LogEventConverter( LogEvent<FromT> const& logEvent );

public:
	LogEvent<ToT> mConverted;

private:
	rftl::basic_string<ToT> mContext;
	rftl::basic_string<ToT> mFormat;
};



template<typename FromT, typename ToT>
LogEventConverter<FromT, ToT>::LogEventConverter( LogEvent<FromT> const& logEvent )
{
	mConverted = {};
	mConverted.mCategoryKey = logEvent.mCategoryKey;
	mConverted.mSeverityMask = logEvent.mSeverityMask;
	mConverted.mLineNumber = logEvent.mLineNumber;
	mConverted.mTransientFileString = logEvent.mTransientFileString;
	if( logEvent.mTransientContextString != nullptr )
	{
		mContext = unicode::ConvertToUtf<ToT>( logEvent.mTransientContextString );
		mConverted.mTransientContextString = mContext.c_str();
	}
	if( logEvent.mTransientMessageFormatString != nullptr )
	{
		mFormat = unicode::ConvertToUtf<ToT>( logEvent.mTransientMessageFormatString );
		mConverted.mTransientMessageFormatString = mFormat.c_str();
	}
}

}
///////////////////////////////////////////////////////////////////////////////

LoggingRouter::LoggingRouter()
	: mHandlerIDGenerator( kInitialHandlerID )
	, mDynamicGlobalWhitelist( kDefaultGlobalWhitelist )
{
	mSeverityStrings.fill( nullptr );
}



void LoggingRouter::Log(
	char const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char const* format, ... ) const
{
	va_list args;
	va_start( args, format );
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void LoggingRouter::Log(
	char16_t const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char16_t const* format, ... ) const
{
	va_list args;
	va_start( args, format );
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void LoggingRouter::Log(
	char32_t const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char32_t const* format, ... ) const
{
	va_list args;
	va_start( args, format );
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
	va_end( args );
}



void LoggingRouter::LogVA(
	char const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char const* format,
	va_list args ) const
{
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
}



void LoggingRouter::LogVA(
	char16_t const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char16_t const* format,
	va_list args ) const
{
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
}



void LoggingRouter::LogVA(
	char32_t const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	char32_t const* format,
	va_list args ) const
{
	LogInternal( context, categoryKey, severityMask, filename, lineNumber, format, args );
}



HandlerID LoggingRouter::RegisterHandler( HandlerDefinition const& handlerDefinition )
{
	WriterLock lock( mMultiReaderSingleWriterLock );

	RF_ASSERT_MSG( handlerDefinition.mSupportedSeverities != 0, "Bad param" );
	RF_ASSERT_MSG(
		handlerDefinition.mUtf8HandlerFunc != nullptr ||
		handlerDefinition.mUtf16HandlerFunc != nullptr ||
		handlerDefinition.mUtf32HandlerFunc != nullptr,
		"Bad param" );

	HandlerID const newID = mHandlerIDGenerator;
	mHandlerIDGenerator++;
	mHandlerDefinitionsByPriority.emplace_back( newID, handlerDefinition );
	return newID;
}



bool LoggingRouter::UnregisterHandler( HandlerID handlerID )
{
	WriterLock lock( mMultiReaderSingleWriterLock );

	bool removed = false;
	HandlerDefinitionsByPriority::const_iterator iter = mHandlerDefinitionsByPriority.begin();
	while( iter != mHandlerDefinitionsByPriority.end() )
	{
		if( iter->first == handlerID )
		{
			RF_ASSERT_MSG( removed == false, "Multiple handlers with the same ID found" );
			iter = mHandlerDefinitionsByPriority.erase( iter );
			removed = true;
		}
		else
		{
			iter++;
		}
	}

	return removed;
}



char const* LoggingRouter::GetStringFromCategoryKey( CategoryKey categoryKey ) const
{
	return categoryKey;
}



void LoggingRouter::SetSeverityString( SeverityMask singleBit, char const* immutableString )
{
	RF_ASSERT_MSG( math::HasOnly1BitSet( singleBit ), "Bad param" );

	WriterLock lock( mMultiReaderSingleWriterLock );

	size_t const index = math::GetZerosIndexOfHighestBit( singleBit );
	mSeverityStrings.at( index ) = immutableString;
}



LoggingRouter::SeverityStrings LoggingRouter::GetSeverityStrings() const
{
	ReaderLock lock( mMultiReaderSingleWriterLock );
	return mSeverityStrings;
}



void LoggingRouter::SetOrModifyGlobalWhitelist( SeverityMask whitelist )
{
	WriterLock lock( mMultiReaderSingleWriterLock );
	mDynamicGlobalWhitelist = whitelist;
}



void LoggingRouter::ClearGlobalWhitelist()
{
	WriterLock lock( mMultiReaderSingleWriterLock );
	mDynamicGlobalWhitelist = kDefaultGlobalWhitelist;
}



void LoggingRouter::SetOrModifyCategoryWhitelist( CategoryKey categoryKey, SeverityMask whitelist )
{
	WriterLock lock( mMultiReaderSingleWriterLock );
	mDynamicCategoryWhitelists[categoryKey] = whitelist;
}



void LoggingRouter::ClearCategoryWhitelist( CategoryKey categoryKey )
{
	WriterLock lock( mMultiReaderSingleWriterLock );
	mDynamicCategoryWhitelists.erase( categoryKey );
}

///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
void LoggingRouter::LogInternal(
	CharT const* context,
	CategoryKey categoryKey,
	SeverityMask severityMask,
	char const* filename,
	size_t lineNumber,
	CharT const* format,
	va_list args ) const
{
	// NOTE: Keeping the lock the whole time, since we also want to not only
	//  protect against our internal variables, but prevent a handler
	//  de-registration to go through, and have the unregisterer think that
	//  it's safe to destroy the handler, since we still need to give it the
	//  log message safely
	// NOTE: If you have a case where your handler MUST be unregisterd
	//  mid-write, then you will need to have a shim mechanism for your handler
	//  to short-circuit the shim, abort, early out safely, and handle this
	//  across all threads. So, not really advisable to get yourself stuck in
	//  that kind of situation...
	ReaderLock lock( mMultiReaderSingleWriterLock );

	RF_ASSERT_MSG( categoryKey != nullptr, "Invalid category key" );
	RF_ASSERT_MSG( severityMask != 0, "Unset severity mask" );

	if( IsDynamicallyFilteredOut( categoryKey, severityMask ) )
	{
		return;
	}

	LogEvent<CharT> logEvent = {};
	logEvent.mCategoryKey = categoryKey;
	logEvent.mSeverityMask = severityMask;
	logEvent.mLineNumber = lineNumber;
	logEvent.mTransientFileString = filename;
	logEvent.mTransientContextString = context;
	logEvent.mTransientMessageFormatString = format;

	for( HandlerDefinitionByID const& handlerDefPair : mHandlerDefinitionsByPriority )
	{
		HandlerDefinition const& handlerDef = handlerDefPair.second;
		if( ( handlerDef.mSupportedSeverities & severityMask ) != 0 )
		{
			LogInternal( logEvent, handlerDef, args );
		}
	}
}



void LoggingRouter::LogInternal( LogEvent<char> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const
{
	if( handlerDef.mUtf8HandlerFunc != nullptr )
	{
		handlerDef.mUtf8HandlerFunc( *this, logEvent, args );
	}
	else if( handlerDef.mUtf16HandlerFunc != nullptr )
	{
		details::LogEventConverter<char, char16_t> converter{ logEvent };
		handlerDef.mUtf16HandlerFunc( *this, converter.mConverted, args );
	}
	else if( handlerDef.mUtf32HandlerFunc != nullptr )
	{
		details::LogEventConverter<char, char32_t> converter{ logEvent };
		handlerDef.mUtf32HandlerFunc( *this, converter.mConverted, args );
	}
	else
	{
		RF_DBGFAIL_MSG( "Invalid handler" );
	}
}



void LoggingRouter::LogInternal( LogEvent<char16_t> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const
{
	if( handlerDef.mUtf8HandlerFunc != nullptr )
	{
		details::LogEventConverter<char16_t, char> converter{ logEvent };
		handlerDef.mUtf8HandlerFunc( *this, converter.mConverted, args );
	}
	else if( handlerDef.mUtf16HandlerFunc != nullptr )
	{
		handlerDef.mUtf16HandlerFunc( *this, logEvent, args );
	}
	else if( handlerDef.mUtf32HandlerFunc != nullptr )
	{
		details::LogEventConverter<char16_t, char32_t> converter{ logEvent };
		handlerDef.mUtf32HandlerFunc( *this, converter.mConverted, args );
	}
	else
	{
		RF_DBGFAIL_MSG( "Invalid handler" );
	}
}



void LoggingRouter::LogInternal( LogEvent<char32_t> const& logEvent, HandlerDefinition const& handlerDef, va_list args ) const
{
	if( handlerDef.mUtf8HandlerFunc != nullptr )
	{
		details::LogEventConverter<char32_t, char> converter{ logEvent };
		handlerDef.mUtf8HandlerFunc( *this, converter.mConverted, args );
	}
	else if( handlerDef.mUtf16HandlerFunc != nullptr )
	{
		details::LogEventConverter<char32_t, char16_t> converter{ logEvent };
		handlerDef.mUtf16HandlerFunc( *this, converter.mConverted, args );
	}
	else if( handlerDef.mUtf32HandlerFunc != nullptr )
	{
		handlerDef.mUtf32HandlerFunc( *this, logEvent, args );
	}
	else
	{
		RF_DBGFAIL_MSG( "Invalid handler" );
	}
}



bool LoggingRouter::IsDynamicallyFilteredOut( CategoryKey categoryKey, SeverityMask severityMask ) const
{
	RF_ASSERT_MSG( categoryKey != nullptr, "Invalid category key" );

	if( ( severityMask & mDynamicGlobalWhitelist ) == 0 )
	{
		return true;
	}

	SeverityMasksByCategoryKey::const_iterator const iter = mDynamicCategoryWhitelists.find( categoryKey );
	if( iter == mDynamicCategoryWhitelists.end() )
	{
		return false;
	}

	if( ( severityMask & iter->second ) == 0 )
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
}
