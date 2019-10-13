#pragma once
#include "project.h"

#include "Rollback/Domain.h"


namespace RF { namespace rollback {
///////////////////////////////////////////////////////////////////////////////

class ROLLBACK_API RollbackManager
{
	RF_NO_COPY( RollbackManager );
	RF_NO_MOVE( RollbackManager );

	//
	// Public methods
public:
	RollbackManager();
	~RollbackManager() = default;

	Domain const& GetSharedDomain() const;
	Domain& GetMutableSharedDomain();

	Domain const& GetPrivateDomain() const;
	Domain& GetMutablePrivateDomain();


	//
	// Private data
private:
	Domain mSharedDomain;
	Domain mPrivateDomain;
};

///////////////////////////////////////////////////////////////////////////////
}}
