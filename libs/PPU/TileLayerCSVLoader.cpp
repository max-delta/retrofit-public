#include "stdafx.h"
#include "TileLayerCSVLoader.h"

#include "PPU/TileLayer.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileBuffer.h"

#include "Serialization/CsvReader.h"

#include "rftl/deque"
#include "rftl/sstream"


namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

bool TileLayerCSVLoader::LoadTiles( gfx::TileLayer& tileLayer, file::VFS const& vfs, file::VFSPath const& path )
{
	file::FileHandlePtr const tilemapHandle = vfs.GetFileForRead( path );
	if( tilemapHandle == nullptr )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to open tilemap" );
		return false;
	}

	file::FileBuffer const tilemapBuffer{ *tilemapHandle.Get(), false };
	if( tilemapBuffer.IsEmpty() )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to load tilemap" );
		return false;
	}

	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( tilemapBuffer.GetChars() );
	if( csv.empty() )
	{
		RFLOG_NOTIFY( path, RFCAT_PPU, "Failed to read tilemap" );
		return false;
	}

	size_t const numRows = csv.size();
	size_t longestRow = 0;
	for( rftl::deque<rftl::string> const& row : csv )
	{
		longestRow = math::Max( longestRow, row.size() );
	}

	tileLayer.ClearAndResize( longestRow, numRows );

	for( size_t i_row = 0; i_row < numRows; i_row++ )
	{
		rftl::deque<rftl::string> const& row = csv.at( i_row );
		size_t const numCols = row.size();

		for( size_t i_col = 0; i_col < numCols; i_col++ )
		{
			rftl::string const& field = row.at( i_col );
			gfx::TileLayer::TileIndex val = gfx::TileLayer::kEmptyTileIndex;
			( rftl::stringstream() << field ) >> val;
			tileLayer.GetMutableTile( i_col, i_row ).mIndex = val;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}}
