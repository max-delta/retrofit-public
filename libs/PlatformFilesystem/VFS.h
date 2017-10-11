#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"
#include "PlatformFilesystem/VFSMount.h"


namespace RF { namespace file {
///////////////////////////////////////////////////////////////////////////////

namespace VFSMountTableTokens
{
constexpr size_t k_NumColumns = 4;

// Serves as prefix and suffix, everything else could be a valid unix
//  identifier, since they're super-permissive
constexpr char k_MountTokenAffix[] = { '/','/' };

// May have \r\n at the end, but we're throwing away the \r anyways
constexpr char k_MountCommentPrefix = '#';
constexpr char k_MountCommentSuffix = '\n';

// Only whitespace can be between tokens and comments, everything else is
//  probably an indication of parser failure
constexpr char k_MountFillerIgnores[] = { ' ','\t','\r','\n' };

constexpr char k_MountTokenAbsolute[] = "mount_abs";
constexpr char k_MountTokenConfigRelative[] = "mount_rel";
constexpr char k_MountTokenUserRelative[] = "mount_usr";

// NOTE: No write+execute, because that's shady, so you'll need overlapping
//  mount rules to permit that explicitly to prove you're not just being
//  lazy and rwx'ing without thinking about the ramifications
constexpr char k_MountTokenReadOnly[] = "ro";
constexpr char k_MountTokenReadWrite[] = "rw";
constexpr char k_MountTokenReadExecute[] = "rx";
}

///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFS
{
	//
	// Types
private:
	typedef std::vector<VFSMount> MountRules;
	typedef std::string MountToken;


	//
	// Constants
public:
	static VFSPath const k_Root;
	static VFSPath const k_Invalid;
	static VFSPath const k_Empty;

	static constexpr char k_PathDelimiter = '/';
	static constexpr char k_PathAscensionElement[] = "..";
	static constexpr char k_PathCurrentElement[] = ".";


	//
	// Public methods
public:
	VFS();

	// HACK: Singleton
	// TODO: Reflection system, system registry
	static VFS* HACK_GetInstance();
	static void HACK_SetInstance( WeakPtr<VFS> instance );

	FileHandlePtr GetFileForRead( VFSPath const& path ) const; // Must exist, seek start
	FileHandlePtr GetFileForWrite( VFSPath const& path ) const; // Will create, seek start
	FileHandlePtr GetFileForModify( VFSPath const& path ) const; // Must exist, seek start
	FileHandlePtr GetFileForAppend( VFSPath const& path ) const; // Will create, seek end
	FileHandlePtr GetFileForExecute( VFSPath const& path ) const; // Must exist, seek start

	// The mount table file contains the base rules for all VFS operations
	// NOTE: Relative entries are relative to the location of the mount file
	// NOTE: User entries are relative to the provided user directory
	// NOTE: Absolute entries are unbounded
	bool AttemptInitialMount( std::string const& mountTableFile, std::string const& userDirectory );

	// File paths may come from outside VFS, such as from OS dialogs, and they
	//  need to be mapped onto VFS layers
	// NOTE: Paths from outside any VFS layers will cause mapping to fail with
	//  an empty VFS path
	VFSPath AttemptMapToVFS( std::string const& physicalPath, VFSMount::Permissions desiredPermissions ) const;

	void DebugDumpMountTable() const;

	static VFSPath CreatePathFromString( std::string const& path );
	static std::string CreateStringFromPath( VFSPath const& path );


	//
	// Private methods
private:
	static VFSPath CollapsePath( VFSPath const& path );
	VFSMount ProcessMountRule( std::string const& type, std::string const& permissions, std::string const& virtualPoint, std::string const& realPoint );
	FileHandlePtr OpenFile( VFSPath const& uncollapsedPath, VFSMount::Permissions const& permissions, char const* openFlags, bool mustExist ) const;


	//
	// Private data
private:
	VFSPath m_MountTableFile;
	VFSPath m_ConfigDirectory;
	VFSPath m_UserDirectory;
	MountRules m_MountTable;
};

///////////////////////////////////////////////////////////////////////////////
}}
