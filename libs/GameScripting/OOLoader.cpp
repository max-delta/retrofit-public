#include "stdafx.h"
#include "OOLoader.h"

#include "Logging/Logging.h"

#include "core_rftype/TypeTraverser.h"
#include "core/meta/ScopedCleanup.h"

#include "rftl/sstream"


namespace RF { namespace script {
///////////////////////////////////////////////////////////////////////////////
namespace details {

struct WorkItem
{
	SquirrelVM::NestedTraversalPath mPath;
	reflect::ClassInfo const* mClassInfo;
	void const* mClassInstance;
};
using WorkItems = rftl::vector<WorkItem>;



rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers(
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> members;

	auto onMemberVariableFunc = [&members](
		rftype::TypeTraverser::MemberVariableInstance const& varInst ) -> void
	{
		members.emplace_back( varInst );
	};

	auto onTraversalTypeFoundFunc = [](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) -> void
	{
		shouldRecurse = false;
	};

	auto onReturnFromTraversalTypeFunc = [](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst ) -> void
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Unexpected recursion" );
	};

	rftype::TypeTraverser::TraverseVariablesT(
		classInfo,
		classInstance,
		onMemberVariableFunc,
		onTraversalTypeFoundFunc,
		onReturnFromTraversalTypeFunc );

	return members;
}



rftl::vector<char const*> GetAllMemberNames(
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const& members )
{
	rftl::vector<char const*> memberNames;
	for( rftype::TypeTraverser::MemberVariableInstance const& member : members )
	{
		memberNames.emplace_back( member.mMemberVariableInfo.mIdentifier );
	}
	return memberNames;
}



bool StoreSingleValueInAccessor(
	size_t key,
	reflect::VariableTypeInfo const& keyInfo,
	reflect::Value const& source,
	reflect::VariableTypeInfo const& targetInfo,
	reflect::ExtensionAccessor const* accessor,
	void* location )
{
	// IMPORTANT! The accessor must already be open for mutation
	// TODO: Check somehow?

	// Make sure it can hold a value
	reflect::Value::Type const targetValueType = targetInfo.mValueType;
	if( targetInfo.mValueType == reflect::Value::Type::Invalid )
	{
		// TODO: Maybe it's a nested class or a void accessor that can somehow
		//  accept it? (Ex: a matching or convertible constructor)
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to convert script value to accessor target variable type" );
		return false;
	}

	reflect::Value const intermediate = source.ConvertTo( targetValueType );
	if( intermediate.GetStoredType() != targetValueType )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to convert script value to accessor target value type" );
		return false;
	}

	void const* const writeableValue = intermediate.GetBytes();
	bool const writeSuccess = accessor->mInsertVariableViaCopy( location, &key, keyInfo, writeableValue, targetInfo );
	if( writeSuccess == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write script value to accessor target" );
		return false;
	}

	return true;
}



bool WriteScriptValueToMemberVariable(
	SquirrelVM::Element const& elemValue,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	RF_ASSERT( rftl::holds_alternative<reflect::Value>( elemValue ) );

	// Figure out where to write to
	void* const destination = const_cast<void*>( member.mMemberVariableLocation );
	reflect::Value::Type const destinationType = member.mMemberVariableInfo.mVariableTypeInfo.mValueType;
	if( destinationType == reflect::Value::Type::Invalid )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Cannot convert script element to an invalid member variable type" );
		return false;
	}

	// Load the script value
	reflect::Value const& source = rftl::get<reflect::Value>( elemValue );

	// Ensure it's compatible
	reflect::Value const intermediate = source.ConvertTo( destinationType );
	if( intermediate.GetStoredType() != destinationType )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to convert script element to member variable type" );
		return false;
	}

	// Write to reflection
	void const* intermediateData = intermediate.GetBytes();
	size_t const intermediateSize = intermediate.GetNumBytes();
	memcpy( destination, intermediateData, intermediateSize );
	return true;
}



bool WriteScriptStringToMemberVariable(
	SquirrelVM::Element const& elemValue,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	RF_ASSERT( rftl::holds_alternative<SquirrelVM::String>( elemValue ) );
	SquirrelVM::String const& string = rftl::get<SquirrelVM::String>( elemValue );

	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = member.mMemberVariableInfo.mVariableTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to convert script array to member variable type" );
		return false;
	}
	void* const location = const_cast<void*>( member.mMemberVariableLocation );

	// Scoped mutation
	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( location );
	}
	auto endMutation = OnScopeEnd( [accessor, location]()
	{
		if( accessor->mEndMutation != nullptr )
		{
			accessor->mEndMutation( location );
		}
	} );

	// For each character...
	size_t indexCounter = 0;
	reflect::VariableTypeInfo keyInfo = {};
	keyInfo.mValueType = reflect::Value::DetermineType<size_t>();
	for( char const& ch : string )
	{
		size_t const key = indexCounter;
		indexCounter++;

		// We'll need to know how to prepare/convert into the target
		reflect::VariableTypeInfo const targetInfo = accessor->mGetVariableTargetInfoByKey( location, &key, keyInfo );

		reflect::Value source{ ch };
		bool const storeSuccess = StoreSingleValueInAccessor( key, keyInfo, source, targetInfo, accessor, location );
		if( storeSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to store script character in accessor" );
			return false;
		}
	}

	return true;
}



bool ProcessElementArrayPopulationWork(
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath currentPath,
	SquirrelVM::ElementArray const& elemArr,
	reflect::ExtensionAccessor const* accessor,
	void* location )
{
	// Scoped mutation
	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( location );
	}
	auto endMutation = OnScopeEnd( [accessor, location]()
	{
		if( accessor->mEndMutation != nullptr )
		{
			accessor->mEndMutation( location );
		}
	} );

	reflect::VariableTypeInfo keyInfo = {};
	keyInfo.mValueType = reflect::Value::DetermineType<size_t>();

	// Pre-create all of our values, just in case we need to queue some work,
	//  and want a stable memory location to write to
	if( accessor->mInsertVariableDefault != nullptr )
	{
		for( size_t i = 0; i < elemArr.size(); i++ )
		{
			bool const writeSuccess = accessor->mInsertVariableDefault( location, &i, keyInfo );
			if( writeSuccess == false )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write reserve value to accessor target" );
				return false;
			}
		}
	}

	// For each value...
	size_t indexCounter = 0;
	for( SquirrelVM::Element const& elemValue : elemArr )
	{
		size_t const key = indexCounter;
		indexCounter++;

		// We'll need to know how to prepare/convert into the target
		reflect::VariableTypeInfo const targetInfo = accessor->mGetVariableTargetInfoByKey( location, &key, keyInfo );

		if( rftl::holds_alternative<reflect::Value>( elemValue ) )
		{
			// Value

			reflect::Value const& source = rftl::get<reflect::Value>( elemValue );

			bool const storeSuccess = StoreSingleValueInAccessor( key, keyInfo, source, targetInfo, accessor, location );
			if( storeSuccess == false )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to store script value in accessor" );
				return false;
			}
		}
		else if( rftl::holds_alternative<SquirrelVM::String>( elemValue ) )
		{
			// String

			// ???Recurse???
			RF_TODO_BREAK();
			return false;
		}
		else if( rftl::holds_alternative<SquirrelVM::ArrayTag>( elemValue ) )
		{
			// Array

			// ???Recurse???
			RF_TODO_BREAK();
			return false;
		}
		else if( rftl::holds_alternative<SquirrelVM::InstanceTag>( elemValue ) )
		{
			// Instance

			if( accessor->mInsertVariableDefault == nullptr )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write any placeholder values to accessor target" );
				return false;
			}

			// Insert a placeholder
			bool const writeSuccess = accessor->mInsertVariableDefault( location, &key, keyInfo );
			if( writeSuccess == false )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write placeholder value to accessor target" );
				return false;
			}

			void const* arrayItemLoc = nullptr;
			reflect::VariableTypeInfo arrayItemInfo = {};
			bool const readSuccess = accessor->mGetVariableTargetByKey( location, &key, keyInfo, arrayItemLoc, arrayItemInfo );
			if( readSuccess == false )
			{
				RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to read placeholder value from accessor target" );
				return false;
			}

			// Queue population as later work
			WorkItem newWorkItem;
			newWorkItem.mClassInfo = arrayItemInfo.mClassInfo;
			newWorkItem.mClassInstance = arrayItemLoc;
			newWorkItem.mPath = currentPath;
			newWorkItem.mPath.emplace_back( ( rftl::stringstream{} << key ).str() );
			workItems.emplace_back( rftl::move( newWorkItem ) );
			return true;
		}
		else
		{
			RF_TODO_BREAK();
			return false;
		}
	}

	return true;
}



bool ProcessScriptArrayPopulationWork(
	SquirrelVM& vm,
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	char const* const elemName,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = member.mMemberVariableInfo.mVariableTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to convert script array to member variable type" );
		return false;
	}
	void* const location = const_cast<void*>( member.mMemberVariableLocation );

	// Load the script values
	SquirrelVM::NestedTraversalPath path = currentWorkItem.mPath;
	path.emplace_back( elemName );
	SquirrelVM::ElementArray const elemArr = vm.GetNestedVariableAsArray( path );

	return ProcessElementArrayPopulationWork( currentWorkItem, workItems, path, elemArr, accessor, location );
}



bool QueueScriptInstancePopulationWork(
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	char const* const elemName,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	reflect::VariableTypeInfo const& targetTypeInfo = member.mMemberVariableInfo.mVariableTypeInfo;
	void const* targetLocation = member.mMemberVariableLocation;

	if( targetTypeInfo.mAccessor != nullptr )
	{
		// Accessor

		// Instantiate
		RF_TODO_BREAK();

		// Queue population as later work
		RF_TODO_BREAK();
		return true;
	}
	else if( targetTypeInfo.mClassInfo != nullptr )
	{
		// Direct nesting

		// Queue population as later work
		WorkItem newWorkItem;
		newWorkItem.mClassInfo = targetTypeInfo.mClassInfo;
		newWorkItem.mClassInstance = targetLocation;
		newWorkItem.mPath = currentWorkItem.mPath;
		newWorkItem.mPath.emplace_back( elemName );
		workItems.emplace_back( rftl::move( newWorkItem ) );
		return true;
	}
	else
	{
		// Unknown

		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to determine handling for script element instance into member variable type" );
		return false;
	}
}



bool ProcessScriptVariable(
	SquirrelVM& vm,
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	char const* const elemName,
	SquirrelVM::Element const& elemValue,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	if( rftl::holds_alternative<reflect::Value>( elemValue ) )
	{
		// Value
		bool const writeSuccess = WriteScriptValueToMemberVariable( elemValue, member );
		if( writeSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write script value '%s' to member variable", elemName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::String>( elemValue ) )
	{
		// String
		bool const writeSuccess = WriteScriptStringToMemberVariable( elemValue, member );
		if( writeSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to write script string '%s' to member variable", elemName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::ArrayTag>( elemValue ) )
	{
		// Array
		bool const processSuccess = ProcessScriptArrayPopulationWork( vm, currentWorkItem, workItems, elemName, member );
		if( processSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to process script array '%s' to member variable", elemName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::InstanceTag>( elemValue ) )
	{
		// Instance
		bool const queueSuccess = QueueScriptInstancePopulationWork( currentWorkItem, workItems, elemName, member );
		if( queueSuccess == false )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Failed to prepare processing for script instance '%s' to be written into member variable", elemName );
			return false;
		}
	}
	else
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Unknown element type on '%s', probably an error in script-handling code", elemName );
		return false;
	}

	return true;
}



bool ProcessWorkItem(
	SquirrelVM& vm,
	WorkItem const& currentWorkItem,
	WorkItems& workItems )
{
	SquirrelVM::NestedTraversalPath const& path = currentWorkItem.mPath;
	reflect::ClassInfo const& classInfo = *currentWorkItem.mClassInfo;
	void const* classInstance = currentWorkItem.mClassInstance;

	// Locally cache the class info from reflection
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers( classInfo, classInstance );

	// Extract the instance from script
	SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
	if( elemMap.empty() )
	{
		RFLOG_WARNING( nullptr, RFCAT_GAMESCRIPTING, "Element map is empty" );
	}

	// For each element...
	for( SquirrelVM::ElementMap::value_type const& elemPair : elemMap )
	{
		// Get the name
		SquirrelVM::String const* elemString = rftl::get_if<SquirrelVM::String>( &( elemPair.first ) );
		char const* const elemName = elemString != nullptr ? elemString->c_str() : nullptr;
		if( elemName == nullptr )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Unable to determine identifier for variable" );
			continue;
		}

		// Find the reflected member with that name
		rftype::TypeTraverser::MemberVariableInstance const* foundMember = nullptr;
		for( rftype::TypeTraverser::MemberVariableInstance const& member : members )
		{
			char const* const memberName = member.mMemberVariableInfo.mIdentifier;
			if( strcmp( elemName, memberName ) == 0 )
			{
				RF_ASSERT( foundMember == nullptr );
				foundMember = &member;
			}
		}
		if( foundMember == nullptr )
		{
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Unable to find matching variable with identifier" );
			continue;
		}
		rftype::TypeTraverser::MemberVariableInstance const& member = *foundMember;

		// Handle the transfer from script->reflect
		SquirrelVM::Element const& elemValue = elemPair.second;
		bool const processSuccess = ProcessScriptVariable( vm, currentWorkItem, workItems, elemName, elemValue, member );
		if( processSuccess == false )
		{
			// TODO: This should probably be conditional whether it
			//  ignores, or aborts the whole process
			continue;
		}
	}

	return true;
}



bool PopulateClassFromScript(
	SquirrelVM& vm,
	SquirrelVM::NestedTraversalPath scriptPath,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	rftl::vector<WorkItem> workItems;

	workItems.emplace_back(
		WorkItem{
			scriptPath,
			&classInfo,
			classInstance } );

	// While there is still work...
	while( workItems.empty() == false )
	{
		// Grab a work item
		WorkItem const workItem = workItems.back();
		workItems.pop_back();

		bool const processSuccess = ProcessWorkItem( vm, workItem, workItems );
		if( processSuccess == false )
		{
			// TODO: This should probably be conditional whether it
			//  ignores, or aborts the whole process
			RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING, "Unable to convert part of a script" );
			continue;
		}
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

OOLoader::OOLoader() = default;
OOLoader::~OOLoader() = default;



bool OOLoader::InjectReflectedClassByClassInfo( reflect::ClassInfo const& classInfo, char const* name )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = details::GetAllMembers( classInfo, nullptr );
	rftl::vector<char const*> const memberNames = details::GetAllMemberNames( members );
	return mVm.InjectSimpleStruct( name, memberNames.data(), memberNames.size() );
}



bool OOLoader::AddSourceFromBuffer( rftl::string const& buffer )
{
	return mVm.AddSourceFromBuffer( buffer );
}



bool OOLoader::AddSourceFromBuffer( char const* buffer, size_t len )
{
	return mVm.AddSourceFromBuffer( buffer, len );
}



bool OOLoader::PopulateClass( char const* rootVariableName, reflect::ClassInfo const& classInfo, void* classInstance )
{
	return PopulateClass( SquirrelVM::NestedTraversalPath{ rootVariableName }, classInfo, classInstance );
}



bool OOLoader::PopulateClass( SquirrelVM::NestedTraversalPath scriptPath, reflect::ClassInfo const& classInfo, void* classInstance )
{
	return details::PopulateClassFromScript( mVm, scriptPath, classInfo, classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}}
