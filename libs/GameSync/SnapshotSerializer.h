#pragma once
#include "project.h"

#include "Rollback/RollbackFwd.h"
#include "PlatformFilesystem/VFSFwd.h"

#include "core/macros.h"


namespace RF { namespace sync {
///////////////////////////////////////////////////////////////////////////////

class GAMESYNC_API SnapshotSerializer final
{
	RF_NO_INSTANCE( SnapshotSerializer );

public:
	static bool SerializeToDiagnosticFile( rollback::Snapshot const& snapshot, file::VFS const& vfs, file::VFSPath const& filename );
};

///////////////////////////////////////////////////////////////////////////////
}}
