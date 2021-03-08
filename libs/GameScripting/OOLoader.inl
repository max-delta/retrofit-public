#pragma once
#include "OOLoader.h"

#include "core_rftype/ClassInfoAccessor.h"


namespace RF::script {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass>
bool OOLoader::InjectReflectedClassByCompileType( char const* name )
{
	return InjectReflectedClassByClassInfo( rftype::GetClassInfo<ReflectedClass>(), name );
}



template<typename ReflectedClass>
bool OOLoader::PopulateClass( char const* rootVariableName, ReflectedClass& classInstance )
{
	return PopulateClass( rootVariableName, rftype::GetClassInfo<ReflectedClass>(), &classInstance );
}



template<typename ReflectedClass>
bool OOLoader::PopulateClass( SquirrelVM::NestedTraversalPath scriptPath, ReflectedClass& classInstance )
{
	return PopulateClass( scriptPath, rftype::GetClassInfo<ReflectedClass>(), &classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}
