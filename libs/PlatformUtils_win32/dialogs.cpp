#include "stdafx.h"
#include "dialogs.h"

#include "core_platform/inc/windows_inc.h"


namespace RF::platform::dialogs {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API rftl::string OpenFileDialog( FileFilters filters )
{
	char filePath[1024];
	filePath[0] = '\0';

	win32::OPENFILENAMEA a = {};
	a.lStructSize = sizeof( win32::OPENFILENAMEA );

	a.lpstrFile = filePath;
	a.nMaxFile = sizeof( filePath );

	using namespace rftl::string_literals;
	rftl::string filterStr = "All files (*.*)\0*.*\0"s;
	for( FileFilter const& filter : filters )
	{
		filterStr.append( filter.first );
		filterStr.append( "\0"s );
		filterStr.append( filter.second );
		filterStr.append( "\0"s );
	}
	a.lpstrFilter = filterStr.c_str();
	a.nFilterIndex = filters.empty() ? 1u : 2u;

	win32::BOOL const result = win32::GetOpenFileNameA( &a );
	if( result == false )
	{
		return rftl::string();
	}

	// Unix-ify the delimiters
	for( char& ch : filePath )
	{
		if( ch == '\0' )
		{
			break;
		}

		if( ch == '\\' )
		{
			ch = '/';
		}
	}

	return rftl::string( filePath );
}



PLATFORMUTILS_API rftl::string SaveFileDialog( FileFilters filters )
{
	char filePath[1024];
	filePath[0] = '\0';

	win32::OPENFILENAMEA a = {};
	a.lStructSize = sizeof( win32::OPENFILENAMEA );

	a.lpstrFile = filePath;
	a.nMaxFile = sizeof( filePath );

	using namespace rftl::string_literals;
	rftl::string filterStr = "All files (*.*)\0*.*\0"s;
	for( FileFilter const& filter : filters )
	{
		filterStr.append( filter.first );
		filterStr.append( "\0"s );
		filterStr.append( filter.second );
		filterStr.append( "\0"s );
	}
	a.lpstrFilter = filterStr.c_str();
	a.nFilterIndex = filters.empty() ? 1u : 2u;

	win32::BOOL const result = win32::GetSaveFileNameA( &a );
	if( result == false )
	{
		return rftl::string();
	}

	// Unix-ify the delimiters
	for( char& ch : filePath )
	{
		if( ch == '\0' )
		{
			break;
		}

		if( ch == '\\' )
		{
			ch = '/';
		}
	}

	return rftl::string( filePath );
}


///////////////////////////////////////////////////////////////////////////////
}
