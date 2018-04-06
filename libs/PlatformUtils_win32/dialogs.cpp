#include "stdafx.h"
#include "dialogs.h"

#include "core_platform/windows_inc.h"


namespace RF { namespace platform { namespace dialogs {
///////////////////////////////////////////////////////////////////////////////

PLATFORMUTILS_API std::string OpenFileDialog()
{
	char filePath[1024];
	filePath[0] = '\0';

	win32::OPENFILENAMEA a = {};
	a.lStructSize = sizeof( win32::OPENFILENAMEA );

	a.lpstrFile = filePath;
	a.nMaxFile = sizeof( filePath );

	a.lpstrFilter = "All\0*.*\0";
	a.nFilterIndex = 1;

	win32::BOOL const result = win32::GetOpenFileNameA( &a );
	if( result == false )
	{
		return std::string();
	}

	// Unix-ify the delimiters
	for( char& ch : filePath )
	{
		if(ch == '\0')
		{
			break;
		}

		if( ch == '\\' )
		{
			ch = '/';
		}
	}

	return std::string( filePath );
}


///////////////////////////////////////////////////////////////////////////////
}}}
