#pragma once
#include "project.h"

#include "PlatformFilesystem/VFSFwd.h"
#include "PlatformFilesystem/VFSMount.h"


namespace RF::file {
///////////////////////////////////////////////////////////////////////////////
namespace VFSMountTableTokens {

constexpr size_t kNumColumns = 4;

// Serves as prefix and suffix, everything else could be a valid unix
//  identifier, since they're super-permissive
constexpr char kMountTokenAffix[] = { '/', '/' };

// May have \r\n at the end, but we're throwing away the \r anyways
constexpr char kMountCommentPrefix = '#';
constexpr char kMountCommentSuffix = '\n';

// Only whitespace can be between tokens and comments, everything else is
//  probably an indication of parser failure
constexpr char kMountFillerIgnores[] = { ' ', '\t', '\r', '\n' };

constexpr char kMountTokenAbsolute[] = "mount_abs";
constexpr char kMountTokenConfigRelative[] = "mount_rel";
constexpr char kMountTokenUserRelative[] = "mount_usr";

// NOTE: No write+execute, because that's shady, so you'll need overlapping
//  mount rules to permit that explicitly to prove you're not just being
//  lazy and rwx'ing without thinking about the ramifications
constexpr char kMountTokenReadOnly[] = "ro";
constexpr char kMountTokenReadWrite[] = "rw";
constexpr char kMountTokenReadExecute[] = "rx";

}
///////////////////////////////////////////////////////////////////////////////

class PLATFORMFILESYSTEM_API VFS
{
	RF_DEFAULT_COPY( VFS );

	//
	// Types
private:
	using MountRules = rftl::vector<VFSMount>;
	using MountToken = rftl::string;


	//
	// Constants
public:
	static VFSPath const kRoot;
	static VFSPath const kInvalid;
	static VFSPath const kEmpty;


	//
	// Public methods
public:
	VFS();
	~VFS();

	FileHandlePtr GetFileForRead( VFSPath const& path ) const; // Must exist, seek start
	FileHandlePtr GetFileForWrite( VFSPath const& path ) const; // Will create, seek start
	FileHandlePtr GetFileForModify( VFSPath const& path ) const; // Must exist, seek start
	FileHandlePtr GetFileForAppend( VFSPath const& path ) const; // Will create, seek end
	FileHandlePtr GetFileForExecute( VFSPath const& path ) const; // Must exist, seek start

	FileHandlePtr GetRawFileForWrite( char const* rawPath ) const; // Will create, seek start

	void EnumerateDirectory(
		VFSPath const& directory,
		VFSMount::Permissions permissions,
		rftl::vector<VFSPath>& files,
		rftl::vector<VFSPath>& folders ) const;

	// The mount table file contains the base rules for all VFS operations
	// NOTE: Relative entries are relative to the location of the mount file
	// NOTE: User entries are relative to the provided user directory
	// NOTE: Absolute entries are unbounded
	bool AttemptInitialMount( rftl::string const& mountTableFile, rftl::string const& userDirectory );
	bool AttemptSubsequentMount( VFSPath const& mountTableFile );

	// File paths may come from outside VFS, such as from OS dialogs, and they
	//  need to be mapped onto VFS layers
	// NOTE: Paths from outside any VFS layers will cause mapping to fail with
	//  an empty VFS path
	VFSPath AttemptMapToVFS( rftl::string const& physicalPath, VFSMount::Permissions desiredPermissions ) const;

	void DebugDumpMountTable() const;


	//
	// Private methods
private:
	static VFSPath CollapsePath( VFSPath const& path );
	static VFSPath ChrootCollapse( VFSPath const& path );
	bool ProcessMountFile( FILE* file );
	VFSMount ProcessMountRule( rftl::string const& type, rftl::string const& permissions, rftl::string const& virtualPoint, rftl::string const& realPoint );
	VFSPath GetRealMountPoint( VFSMount const& mount ) const;
	rftl::string AttemptMountMapping( VFSMount const& mount, VFSPath const& collapsedPath, VFSMount::Permissions const& permissions ) const;
	FileHandlePtr OpenFile( VFSPath const& uncollapsedPath, VFSMount::Permissions const& permissions, char const* openFlags, bool mustExist ) const;


	//
	// Private data
private:
	VFSPath mMountTableFile;
	VFSPath mConfigDirectory;
	VFSPath mUserDirectory;
	MountRules mMountTable;
};

///////////////////////////////////////////////////////////////////////////////
}
