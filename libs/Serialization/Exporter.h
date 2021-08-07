#pragma once
#include "project.h"

#include "core_math/Hash.h"
#include "core_reflect/Value.h"

#include "core/macros.h"


namespace RF::serialization {
///////////////////////////////////////////////////////////////////////////////

class SERIALIZATION_API Exporter
{
	RF_NO_COPY( Exporter );

public:
	using InstanceID = uint64_t;
	using TypeID = math::HashVal64;
	using IndirectionID = uint64_t;
	using ExternalReferenceID = char const*;


public:
	Exporter() = default;
	virtual ~Exporter() = default;

	// Completes the current export, preventing any further actions from being
	//  performed on it
	virtual bool Root_FinalizeExport() = 0;

	// A new instance at the root-level, such as a class/struct
	// NOTE: Indicates a close of the most recent root-level type
	virtual bool Root_BeginNewInstance() = 0;

	// Denotes the relationship between an indirection ID and an instance ID
	//  that can be resolved locally in the current export
	virtual bool Root_RegisterLocalIndirection( IndirectionID const& indirectionID, InstanceID const& instanceID ) = 0;

	// Denotes the relationship between an indirection ID and an instance ID
	//  that can be resolved externally using a reference ID
	virtual bool Root_RegisterExternalIndirection( IndirectionID const& indirectionID, ExternalReferenceID const& referenceID ) = 0;

	// Add an attribute to the current instance to indicate its ID
	// NOTE: Invalid if a property has begun on the current instance
	// NOTE: Invalid if another instance in the current export shares the ID
	virtual bool Instance_AddInstanceIDAttribute( InstanceID const& instanceID ) = 0;

	// Add an attribute to the current instance to indicate its type
	// NOTE: Debug name not required to be stored by exporter, and not required
	//  to be provided by caller
	// NOTE: Invalid if a property has begun on the current instance
	virtual bool Instance_AddTypeIDAttribute( TypeID const& typeID, char const* debugName ) = 0;

	// A new property in the current instance at the current level, such as a
	//  member variable
	// NOTE: Indicates a close of the most recent property in the current level
	// NOTE: Invalid if no instance has begun
	virtual bool Instance_BeginNewProperty() = 0;

	// Add an attribute to the current property to indicate its name
	// NOTE: Invalid if no property has begun at the current level
	// NOTE: Invalid if the current property has performed an indentation
	virtual bool Property_AddNameAttribute( char const* name ) = 0;

	// Add an attribute to the current property to indicate its value
	// NOTE: Invalid if no property has begun at the current level
	// NOTE: Invalid if the current property has performed an indentation
	virtual bool Property_AddValueAttribute( reflect::Value const& value ) = 0;

	// Add an attribute to the current property to indicate it requires
	//  indirection in order to resolve
	// NOTE: Invalid if no property has begun at the current level
	// NOTE: Invalid if the current property has performed an indentation
	// NOTE: Invalid for the current property to performed an indentation if it
	//  has this attribute
	virtual bool Property_AddIndirectionAttribute( IndirectionID const& indirectionID ) = 0;

	// Begins a new level in the current instance at the current level,
	//  branching from the most recent property, such as the contents of a
	//  complex member variable that is not accessed via indirection
	// NOTE: Invalid if no property has begun at the current level
	virtual bool Property_IndentFromCurrentProperty() = 0;

	// Ends the current level in the current instance, returning to the most
	//  recent property
	// NOTE: Invalid if at the top level of an instance
	virtual bool Property_OutdentFromLastIndent() = 0;
};

///////////////////////////////////////////////////////////////////////////////
}
