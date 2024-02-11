#include "stdafx.h"

#include "bindump/BinDump.h"

#include "core_math/math_casts.h"


///////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[] )
{
	using namespace RF;
	using namespace RF::bindump;

	// Init
	{
		ErrorReturnCode const initResult = Init( { argc, argv } );
		if( initResult != ErrorReturnCode::Success )
		{
			return Shutdown( initResult );
		}
	}

	// Process
	{
		ErrorReturnCode const processResult = Process();
		if( processResult != ErrorReturnCode::Success )
		{
			return Shutdown( processResult );
		}
	}

	return Shutdown( ErrorReturnCode::Success );
}
