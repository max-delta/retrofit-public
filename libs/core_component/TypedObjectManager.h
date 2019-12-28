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
	TypedObjectManager() = delete;
	TypedObjectManager( ManagerIdentifier identifier, ScopeIdentifier generationScope, TypeResolver&& typeResolver );
	TypedObjectManager( ManagerIdentifier identifier, TypeResolver&& typeResolver );

	// Type resolver provides options for talking about components besides just
	//  the basic component identifier
	TypeResolver const& GetTypeResolver() const;

	template<typename ComponentType>
	static ResolvedComponentType ResolveComponentType( TypeResolver const& resolver );
	static ResolvedComponentType ResolveComponentType( TypeResolver const& resolver, ComponentLabel const& label );
	static bool IsSafeConversion( TypeResolver const& resolver, ComponentRef const& from, ResolvedComponentType to );

	TypedObjectRef<TypeResolver> GetObject( ObjectIdentifier identifier ) const;
	TypedMutableObjectRef<TypeResolver> GetMutableObject( ObjectIdentifier identifier );
	TypedMutableObjectRef<TypeResolver> AddObject();

	template<typename ComponentType> bool IsValidComponentT( ObjectIdentifier identifier ) const;
	template<typename ComponentType> TypedComponentRef<TypeResolver> GetComponentT( ObjectIdentifier identifier ) const;
	template<typename ComponentType> TypedMutableComponentRef<TypeResolver> GetMutableComponentT( ObjectIdentifier identifier );
	template<typename ComponentType> TypedMutableComponentRef<TypeResolver> AddComponentT( ObjectIdentifier identifier, ComponentInstance&& instance );
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
