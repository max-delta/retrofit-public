#include "stdafx.h"
#include "ArgView.h"

#include "core_math/math_casts.h"


namespace RF::cli {
///////////////////////////////////////////////////////////////////////////////

ArgView::ArgView( int argc, char* argv[] )
	: mArgC( math::integer_cast<size_t>( argc ) )
	, mArgV( argv )
{
	//
}



ArgView::ArgView( size_t argc, value_type argv[] )
	: mArgC( argc )
	, mArgV( argv )
{
	//
}



ArgView::value_type* ArgView::begin() const
{
	if( mArgV == nullptr )
	{
		return nullptr;
	}

	return &mArgV[1];
}



ArgView::value_type* ArgView::end() const
{
	if( mArgV == nullptr )
	{
		return nullptr;
	}

	return &mArgV[mArgC];
}

///////////////////////////////////////////////////////////////////////////////
}
