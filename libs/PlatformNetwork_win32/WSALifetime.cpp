#include "stdafx.h"

#include "core_platform/winsock2_inc.h"
#pragma comment( lib, "Ws2_32.lib" )


namespace RF::platform::network {
///////////////////////////////////////////////////////////////////////////////

struct WSALifetime
{
	WSALifetime();
	~WSALifetime();
};

static size_t sWSAStartupSuccess = 0;
RF_NO_LINK_STRIP WSALifetime sWSALifetime = {};


///////////////////////////////////////////////////////////////////////////////

WSALifetime::WSALifetime()
{
	// Version 2.2.x, just because that's what MS has recommended for decades
	static constexpr win32::WORD kVersion = ( 2 << 8 ) | 2;

	win32::WSADATA wsaData = {};
	int const result = win32::WSAStartup( kVersion, &wsaData );
	if( result != 0 )
	{
		// This is non-fatal, but is highly unexpected and means that most
		//  networking logic will fail
		RF_DBGFAIL_MSG( "Failed to start WinSock" );
	}
	else
	{
		sWSAStartupSuccess++;
	}
}



WSALifetime::~WSALifetime()
{
	if( sWSAStartupSuccess > 0 )
	{
		int const result = win32::WSACleanup();
		if( result != 0 )
		{
			// This is non-fatal, but is highly unexpected and probably means
			//  something has been corrupted, or someone was calling startup or
			//  cleanup on their own (which they should not have been doing)
			RF_DBGFAIL_MSG( "Failed to stop WinSock" );
		}

		// Assume success, error handling unclear here
		sWSAStartupSuccess--;
	}
}

///////////////////////////////////////////////////////////////////////////////
}
