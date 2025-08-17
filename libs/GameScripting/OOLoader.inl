#pragma once
#include "OOLoader.h"

#include "core_rftype/ClassInfoAccessor.h"


namespace RF::script {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass>
bool OOLoader::InjectReflectedClassByCompileType( rftl::string_view name )
{
	return InjectReflectedClassByClassInfo( rftype::GetClassInfo<ReflectedClass>(), name );
}



template<typename ReflectedClass>
bool OOLoader::PopulateClass( rftl::string_view rootVariableName, ReflectedClass& classInstance )
{
	static_assert( rftl::is_const<ReflectedClass>::value == false );
	return PopulateClass( rootVariableName, rftype::GetClassInfo<ReflectedClass>(), &classInstance );
}



template<typename ReflectedClass>
bool OOLoader::PopulateClass( SquirrelVM::NestedTraversalPath scriptPath, ReflectedClass& classInstance )
{
	static_assert( rftl::is_const<ReflectedClass>::value == false );
	return PopulateClass( scriptPath, rftype::GetClassInfo<ReflectedClass>(), &classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}
