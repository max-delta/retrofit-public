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
		RF_DBGFAIL();
		return false;
	}

	file::FileBuffer const tilemapBuffer{ *tilemapHandle.Get(), false };
	if( tilemapBuffer.GetData() == nullptr )
	{
		RF_DBGFAIL();
		return false;
	}

	char const* const data = reinterpret_cast<char const*>( tilemapBuffer.GetData() );
	size_t const size = tilemapBuffer.GetSize();
	rftl::deque<rftl::deque<rftl::string>> const csv = serialization::CsvReader::TokenizeToDeques( data, size );
	if( csv.empty() )
	{
		RF_DBGFAIL();
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