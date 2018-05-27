#include "stdafx.h"
#include "TEMP_AssetLoadSave.h"

#include "AppCommon_GraphicalClient/Common.h"

#include "PPU/PPUController.h"
#include "PPU/FramePack.h"
#include "PPU/TextureManager.h"

#include "PlatformFilesystem/VFS.h"
#include "PlatformFilesystem/FileHandle.h"
#include "Scripting_squirrel/squirrel.h"

#include "core/ptr/default_creator.h"
#include "core_math/math_casts.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

RF::UniquePtr<RF::gfx::FramePackBase> LoadFramePackFromSquirrel( RF::file::VFSPath const& filename, size_t minFrames )
{
	using namespace RF;

	WeakPtr<gfx::TextureManager> texMan = app::g_Graphics->DebugGetTextureManager();
	script::SquirrelVM vm;
	script::SquirrelVM::Element elem;
	script::SquirrelVM::ElementArray elemArr;
	script::SquirrelVM::Integer const* integer;
	UniquePtr<gfx::FramePackBase> retVal;

	{
		rftl::string fileBuf;
		{
			file::FileHandlePtr const digitFPackFilePtr = app::g_Vfs->GetFileForRead( filename );
			RF_ASSERT( digitFPackFilePtr != nullptr );
			FILE* const digitFPackFile = digitFPackFilePtr->GetFile();
			RF_ASSERT( digitFPackFile != nullptr );

			long filesize;
			{
				fseek( digitFPackFile, 0, SEEK_END );
				filesize = ftell( digitFPackFile );
				rewind( digitFPackFile );
			}
			fileBuf.reserve( math::integer_cast<size_t>( filesize ) );

			int fch = 0;
			while( ( fch = fgetc( digitFPackFile ) ) != EOF )
			{
				fileBuf.push_back( math::integer_cast<char>( fch ) );
			}
		}

		bool const sourceSuccess = vm.AddSourceFromBuffer( fileBuf );
		RF_ASSERT( sourceSuccess );
	}

	elem = vm.GetGlobalVariable( "NumTimeSlots" );
	integer = rftl::get_if<script::SquirrelVM::Integer>( &elem );
	RF_ASSERT( integer != nullptr );
	size_t const minFramesToCreate = math::Max<size_t>( minFrames, math::integer_cast<size_t>( *integer ) );
	if( minFramesToCreate <= gfx::FramePack_256::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_256>::Create();
	}
	else if( minFramesToCreate <= gfx::FramePack_512::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_512>::Create();
	}
	else if( minFramesToCreate <= gfx::FramePack_1024::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_1024>::Create();
	}
	else if( minFramesToCreate <= gfx::FramePack_4096::k_MaxTimeSlots )
	{
		retVal = DefaultCreator<gfx::FramePack_4096>::Create();
	}
	else
	{
		RF_DBGFAIL();
	}
	retVal->m_NumTimeSlots = math::integer_cast<uint8_t>( *integer );

	elem = vm.GetGlobalVariable( "PreferredSlowdownRate" );
	integer = rftl::get_if<script::SquirrelVM::Integer>( &elem );
	RF_ASSERT( integer != nullptr );
	retVal->m_PreferredSlowdownRate = math::integer_cast<uint8_t>( *integer );

	elemArr = vm.GetGlobalVariableAsArray( "Texture" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		rftl::string const* string = rftl::get_if<script::SquirrelVM::String>( &elemRef );
		RF_ASSERT( string != nullptr );
		retVal->GetMutableTimeSlots()[i].m_TextureReference =
			texMan->LoadNewResourceGetID(
				file::VFS::CreateStringFromPath( filename ).append( { static_cast<char>( i + 1 ),'\0' } ),
				file::VFS::k_Root.GetChild( file::VFS::CreatePathFromString( *string ) ) );
	}

	elemArr = vm.GetGlobalVariableAsArray( "TextureOriginX" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		integer = rftl::get_if<script::SquirrelVM::Integer>( &elemRef );
		RF_ASSERT( integer != nullptr );
		retVal->GetMutableTimeSlots()[i].m_TextureOriginX = math::integer_cast<uint8_t>( *integer );
	}

	elemArr = vm.GetGlobalVariableAsArray( "TextureOriginY" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		integer = rftl::get_if<script::SquirrelVM::Integer>( &elemRef );
		RF_ASSERT( integer != nullptr );
		retVal->GetMutableTimeSlots()[i].m_TextureOriginY = math::integer_cast<uint8_t>( *integer );
	}

	elemArr = vm.GetGlobalVariableAsArray( "Sustain" );
	for( size_t i = 0; i < elemArr.size(); i++ )
	{
		script::SquirrelVM::Element const& elemRef = elemArr[i];
		integer = rftl::get_if<script::SquirrelVM::Integer>( &elemRef );
		RF_ASSERT( integer != nullptr );
		retVal->GetMutableTimeSlotSustains()[i] = math::integer_cast<uint8_t>( *integer );
	}

	return retVal;
}

///////////////////////////////////////////////////////////////////////////////
}
