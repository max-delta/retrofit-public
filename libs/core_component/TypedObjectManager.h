#pragma once
#include "core_component/ObjectManager.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
class TypedObjectManager : public ObjectManager
{
	RF_NO_COPY( TypedObjectManager );

	//
	// Public methods
public:
	TypedObjectManager( ManagerIdentifier identifier, TypeResolver&& typeResolver );

	// Type resolver provides options for talking about components besides just
	//  the basic component identifier
	TypeResolver const& GetTypeResolver() const;

	template<typename ComponentType>
	ResolvedComponentType ResolveComponentType() const;
	ResolvedComponentType ResolveComponentType( ComponentLabel const& label ) const;


	//
	// Private data
private:
	TypeResolver mTypeResolver;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypedObjectManager.inl"
