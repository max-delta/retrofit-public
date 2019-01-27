#include "stdafx.h"
#include "Font.h"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

Font::Font()
	: mFileBuffer( ExplicitDefaultConstruct() )
{
	//
}



Font::~Font()
{
	// TODO: De-ref texture?
}



DeviceFontID Font::GetDeviceRepresentation() const
{
	return mDeviceRepresentation;
}



uint32_t Font::DebugGetWidth() const
{
	return mTileWidth;
}



uint32_t Font::DebugGetHeight() const
{
	return mTileHeight;
}

///////////////////////////////////////////////////////////////////////////////
}}
