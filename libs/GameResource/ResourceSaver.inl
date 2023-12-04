#pragma once
#include "ResourceSaver.h"

#include "core_rftype/ClassInfoAccessor.h"


namespace RF::resource {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass>
inline bool ResourceSaver::SaveClassToFile(
	ResourceTypeIdentifier typeID,
	ReflectedClass const& classInstance,
	file::VFSPath const& path )
{
	static_assert( rftl::is_const<ReflectedClass>::value == false );
	return SaveClassToFile(
		rftype::GetClassInfo<ReflectedClass>(),
		&classInstance,
		typeID,
		path );
}



template<typename ReflectedClass>
inline bool ResourceSaver::SaveClassToBuffer(
	ResourceTypeIdentifier typeID,
	ReflectedClass const& classInstance,
	rftl::string& buffer )
{
	static_assert( rftl::is_const<ReflectedClass>::value == false );
	return SaveClassToBuffer(
		rftype::GetClassInfo<ReflectedClass>(),
		&classInstance,
		typeID,
		buffer );
}

///////////////////////////////////////////////////////////////////////////////
}
