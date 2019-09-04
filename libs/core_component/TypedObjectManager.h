#pragma once
#include "core_component/ObjectManager.h"


namespace RF { namespace component {
///////////////////////////////////////////////////////////////////////////////

template<typename TypeResolver>
class TypedObjectManager : public ObjectManager
{
	RF_NO_COPY( TypedObjectManager );

	//
	// Types
public:
	template<typename ComponentType> using ComponentInstanceRefT = WeakPtr<ComponentType const>;
	template<typename ComponentType> using MutableComponentInstanceRefT = WeakPtr<ComponentType>;

	//
	// Public methods
public:
	TypedObjectManager() = delete;
	TypedObjectManager( ManagerIdentifier identifier, ScopeIdentifier generationScope, TypeResolver&& typeResolver );
	TypedObjectManager( ManagerIdentifier identifier, TypeResolver&& typeResolver );

	// Type resolver provides options for talking about components besides just
	//  the basic component identifier
	TypeResolver const& GetTypeResolver() const;

	template<typename ComponentType>
	ResolvedComponentType ResolveComponentType() const;
	ResolvedComponentType ResolveComponentType( ComponentLabel const& label ) const;
	bool IsSafeConversion( ComponentRef const& from, ResolvedComponentType to ) const;

	template<typename ComponentType> bool IsValidComponentT( ObjectIdentifier identifier ) const;
	template<typename ComponentType> ComponentRef GetComponentT( ObjectIdentifier identifier ) const;
	template<typename ComponentType> MutableComponentRef GetMutableComponentT( ObjectIdentifier identifier );
	template<typename ComponentType> MutableComponentRef AddComponentT( ObjectIdentifier identifier, ComponentInstance&& instance );
	template<typename ComponentType> bool RemoveComponentT( ObjectIdentifier identifier );

	template<typename ComponentType> ComponentInstanceRefT<ComponentType> GetComponentInstanceT( ComponentRef const& component ) const;
	template<typename ComponentType> MutableComponentInstanceRefT<ComponentType> GetMutableComponentInstanceT( MutableComponentRef const& component );


	//
	// Private data
private:
	TypeResolver mTypeResolver;
};

///////////////////////////////////////////////////////////////////////////////
}}

#include "TypedObjectManager.inl"
