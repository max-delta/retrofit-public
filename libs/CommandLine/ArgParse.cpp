#include "stdafx.h"
#include "ArgParse.h"

#include "CommandLine/ArgView.h"

#include "rftl/string_view"


namespace RF::cli {
///////////////////////////////////////////////////////////////////////////////

ArgParse::ArgParse( ArgView const& args )
{
	Parse( args );
}



ArgParse::OptionNames ArgParse::GetOptionNames() const
{
	OptionNames retVal;
	retVal.reserve( mOptions.size() );
	for( Options::value_type const& option : mOptions )
	{
		retVal.emplace( option.first );
	}
	return retVal;
}



bool ArgParse::HasOption( OptionName const& name ) const
{
	return mOptions.count( name ) > 0;
}



ArgParse::OptionVals ArgParse::GetOptionVals( OptionName const& name ) const
{
	Options::const_iterator const iter = mOptions.find( name );
	if( iter != mOptions.end() )
	{
		return iter->second;
	}
	return {};
}



ArgParse::Arguments ArgParse::GetArguments() const
{
	return mArguments;
}

///////////////////////////////////////////////////////////////////////////////

void ArgParse::Parse( ArgView const& args )
{
	Options::iterator curOption = mOptions.end();
	auto const hasCurOption = [this, &curOption]() -> bool //
	{
		return curOption != mOptions.end();
	};

	bool onlyArguments = false;

	// For each parameter...
	for( ArgView::value_type const& viewParam : args )
	{
		rftl::string_view const param( viewParam );
		RF_ASSERT( param.empty() == false );

		if( onlyArguments )
		{
			// No options allowed anymore, treat as arg
			mArguments.emplace_back( param );
			continue;
		}

		if( param == kOptionTerminator )
		{
			// Once options are terminated, every param becomes an argument
			onlyArguments = true;
			continue;
		}

		if( param.size() > 1 && param.front() == kOptionPrefix )
		{
			// Start a new option
			curOption = mOptions.try_emplace( rftl::string( param ) ).first;
			continue;
		}

		if( hasCurOption() )
		{
			// Store as option
			curOption->second.emplace_back( param );
			continue;
		}

		// No option is active, treat as arg
		mArguments.emplace_back( param );
		continue;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
