#include "stdafx.h"
#include "OOLoader.h"

#include "Logging/Logging.h"

#include "core_rftype/ConstructedType.h"
#include "core_rftype/TypeTraverser.h"
#include "core_rftype/VirtualCast.h"
#include "core_unicode/StringConvert.h"

#include "core/meta/ScopedCleanup.h"
#include "core/ptr/default_creator.h"

#include "rftl/extension/string_copy.h"
#include "rftl/extension/c_str.h"
#include "rftl/sstream"


template<>
void RF::logging::WriteContextString( rftype::TypeTraverser::MemberVariableInstance const& context, Utf8LogContextBuffer& buffer )
{
	rftl::string_copy( buffer, RF::unicode::ConvertToUtf8( rftl::string_view( context.mMemberVariableInfo.mIdentifier ) ) );
}

namespace RF::script {
///////////////////////////////////////////////////////////////////////////////
namespace details {


RF_ACK_AGGREGATE_NOCOPY();
struct ScratchLookup
{
	OOLoader::InjectedClasses const& mInjectedClasses;

	OOLoader::TypeConstructor const& mTypeConstructor;
};



struct TargetItem
{
	reflect::VariableTypeInfo mTypeInfo;
	void const* mLocation = nullptr;
	char const* mIdentifier = nullptr;
};



struct WorkItem
{
	SquirrelVM::NestedTraversalPath mPath;
	TargetItem mTarget;
};
using WorkItems = rftl::vector<WorkItem>;



reflect::ClassInfo const* FindClassInfoForName(
	ScratchLookup const& scratchLookup,
	rftl::string_view const& name )
{
	RF_ASSERT( name.empty() == false );

	for( OOLoader::InjectedClass const& injectedClass : scratchLookup.mInjectedClasses )
	{
		RF_ASSERT( injectedClass.mClassInfo != nullptr );
		RF_ASSERT( injectedClass.mName.empty() == false );
		if( injectedClass.mName == name )
		{
			return injectedClass.mClassInfo;
		}
	}

	return nullptr;
}



rftype::ConstructedType ConstructClassByClassName(
	ScratchLookup const& scratchLookup,
	rftl::string_view const& className )
{
	RF_ASSERT( className.empty() == false );

	if( scratchLookup.mTypeConstructor == nullptr )
	{
		RFLOG_NOTIFY( className, RFCAT_GAMESCRIPTING,
			"Cannot construct class, no type constructor registered" );
		return {};
	}

	reflect::ClassInfo const* const classInfo = FindClassInfoForName( scratchLookup, className );
	if( classInfo == nullptr )
	{
		RFLOG_NOTIFY( className, RFCAT_GAMESCRIPTING,
			"No class info for name" );
		return {};
	}

	rftype::ConstructedType constructedType = scratchLookup.mTypeConstructor( *classInfo );
	if( constructedType.mLocation == nullptr )
	{
		RFLOG_NOTIFY( className, RFCAT_GAMESCRIPTING,
			"Type constructor failed to construct type" );
		return {};
	}
	RF_ASSERT( constructedType.mClassInfo != nullptr );
	RF_ASSERT( constructedType.mClassInfo == classInfo );
	return constructedType;
}



rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers(
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> members;

	auto onMemberVariableFunc = [&members] //
		( rftype::TypeTraverser::MemberVariableInstance const& varInst ) -> void //
	{
		members.emplace_back( varInst );
	};

	auto onTraversalTypeFoundFunc = //
		[](
			rftype::TypeTraverser::TraversalType traversalType,
			rftype::TypeTraverser::TraversalVariableInstance const& varInst,
			bool& shouldRecurse ) -> void
	{
		shouldRecurse = false;
	};

	auto onReturnFromTraversalTypeFunc = //
		[](
			rftype::TypeTraverser::TraversalType traversalType,
			rftype::TypeTraverser::TraversalVariableInstance const& varInst ) -> void
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING,
			"Unexpected recursion" );
	};

	rftype::TypeTraverser::TraverseVariablesT(
		classInfo,
		classInstance,
		onMemberVariableFunc,
		onTraversalTypeFoundFunc,
		onReturnFromTraversalTypeFunc );

	return members;
}



rftl::vector<rftl::string_view> GetAllMemberNames(
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const& members )
{
	rftl::vector<rftl::string_view> memberNames;
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
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING,
			"Failed to convert script value to accessor target variable type"
			" (more errors should follow with more details)" );
		return false;
	}

	reflect::Value const intermediate = source.ConvertTo( targetValueType );
	if( intermediate.GetStoredType() != targetValueType )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING,
			"Failed to convert script value to accessor target value type"
			" (more errors should follow with more details)" );
		return false;
	}

	rftl::byte_view const writeableValue = intermediate.GetBytes();
	bool const writeSuccess = accessor->mInsertVariableViaCopy( location, &key, keyInfo, writeableValue.data(), targetInfo );
	if( writeSuccess == false )
	{
		RFLOG_NOTIFY( nullptr, RFCAT_GAMESCRIPTING,
			"Failed to write script value to accessor target" );
		return false;
	}

	return true;
}



bool WriteScriptValueToVariable(
	SquirrelVM::Element const& elemValue,
	TargetItem const& target )
{
	RF_ASSERT( rftl::holds_alternative<reflect::Value>( elemValue ) );

	// Figure out where to write to
	void* const destination = const_cast<void*>( target.mLocation );
	reflect::Value::Type const destinationType = target.mTypeInfo.mValueType;
	if( destinationType == reflect::Value::Type::Invalid )
	{
		if( target.mTypeInfo.mAccessor != nullptr )
		{
			RFLOG_NOTIFY( target.mIdentifier, RFCAT_GAMESCRIPTING,
				"Cannot convert script element to an invalid member variable type"
				" (An accessor is available, script probably is improperly"
				" using a simpler value type, which might also happen"
				" implicitly if the variable was never set in script)" );
		}
		else
		{
			RFLOG_NOTIFY( target.mIdentifier, RFCAT_GAMESCRIPTING,
				"Cannot convert script element to an invalid member variable type"
				" (No accessor is present either, this is probably a registration failure in code)" );
		}
		return false;
	}

	// Load the script value
	reflect::Value const& source = rftl::get<reflect::Value>( elemValue );

	// Ensure it's compatible
	reflect::Value const intermediate = source.ConvertTo( destinationType );
	if( intermediate.GetStoredType() != destinationType )
	{
		RFLOG_NOTIFY( target.mIdentifier, RFCAT_GAMESCRIPTING,
			"Failed to convert script element to member variable type" );
		return false;
	}

	// Write to reflection
	rftl::byte_view const intermediateBytes = intermediate.GetBytes();
	intermediateBytes.mem_copy_to( destination, reflect::Value::GetNumBytesNeeded( destinationType ) );
	return true;
}



bool WriteScriptStringToVariable(
	SquirrelVM::Element const& elemValue,
	TargetItem const& target )
{
	RF_ASSERT( rftl::holds_alternative<SquirrelVM::String>( elemValue ) );
	SquirrelVM::String const& string = rftl::get<SquirrelVM::String>( elemValue );

	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = target.mTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RFLOG_NOTIFY( target.mIdentifier, RFCAT_GAMESCRIPTING,
			"Failed to convert script array to member variable type"
			" (more errors should follow with more details)" );
		return false;
	}
	void* const location = const_cast<void*>( target.mLocation );

	// Scoped mutation
	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( location );
	}
	auto endMutation = OnScopeEnd(
		[accessor, location]() //
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
		reflect::VariableTypeInfo const targetInfo = accessor->mGetTargetInfoByKey( location, &key, keyInfo );

		reflect::Value source{ ch };
		bool const storeSuccess = StoreSingleValueInAccessor( key, keyInfo, source, targetInfo, accessor, location );
		if( storeSuccess == false )
		{
			RFLOG_NOTIFY( target.mIdentifier, RFCAT_GAMESCRIPTING,
				"Failed to store script character in accessor"
				" (more errors should follow with more details)" );
			return false;
		}
	}

	return true;
}



bool ProcessElementArrayPopulationWork(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	SquirrelVM::ElementArray const& elemArr,
	reflect::ExtensionAccessor const* accessor,
	void* location )
{
	// Scoped mutation
	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( location );
	}
	auto endMutation = OnScopeEnd(
		[accessor, location]() //
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
				RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
					"Failed to write reserve value to accessor target" );
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

		SquirrelVM::NestedTraversalPath const targetPath = rftl::concatenate( currentPath, ( rftl::stringstream{} << key ).str() );

		// We'll need to know how to prepare/convert into the target
		reflect::VariableTypeInfo const targetInfo = accessor->mGetTargetInfoByKey( location, &key, keyInfo );

		if( rftl::holds_alternative<reflect::Value>( elemValue ) )
		{
			// Value

			reflect::Value const& source = rftl::get<reflect::Value>( elemValue );

			bool const storeSuccess = StoreSingleValueInAccessor( key, keyInfo, source, targetInfo, accessor, location );
			if( storeSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to store script value in accessor" );
				return false;
			}
		}
		else if( rftl::holds_alternative<SquirrelVM::String>( elemValue ) )
		{
			// String

			if( accessor->mInsertVariableDefault == nullptr )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to write any placeholder values to accessor target" );
				return false;
			}

			// Insert a placeholder
			bool const writeSuccess = accessor->mInsertVariableDefault( location, &key, keyInfo );
			if( writeSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to write placeholder value to accessor target" );
				return false;
			}

			void const* arrayItemLoc = nullptr;
			reflect::VariableTypeInfo arrayItemInfo = {};
			bool const readSuccess = accessor->mGetTargetByKey( location, &key, keyInfo, arrayItemLoc, arrayItemInfo );
			if( readSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to read placeholder value from accessor target" );
				return false;
			}

			// Queue population as later work
			WorkItem newWorkItem;
			newWorkItem.mTarget.mTypeInfo = arrayItemInfo;
			newWorkItem.mTarget.mLocation = arrayItemLoc;
			newWorkItem.mTarget.mIdentifier = "STRING_ARRAY";
			newWorkItem.mPath = targetPath;
			workItems.emplace_back( rftl::move( newWorkItem ) );
			continue;
		}
		else if( rftl::holds_alternative<SquirrelVM::ArrayTag>( elemValue ) )
		{
			// Array

			if( accessor->mInsertVariableDefault == nullptr )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to write any placeholder values to accessor target" );
				return false;
			}

			// Insert a placeholder
			bool const writeSuccess = accessor->mInsertVariableDefault( location, &key, keyInfo );
			if( writeSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to write placeholder value to accessor target" );
				return false;
			}

			void const* arrayItemLoc = nullptr;
			reflect::VariableTypeInfo arrayItemInfo = {};
			bool const readSuccess = accessor->mGetTargetByKey( location, &key, keyInfo, arrayItemLoc, arrayItemInfo );
			if( readSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to read placeholder value from accessor target" );
				return false;
			}

			// Queue population as later work
			WorkItem newWorkItem;
			newWorkItem.mTarget.mTypeInfo = arrayItemInfo;
			newWorkItem.mTarget.mLocation = arrayItemLoc;
			newWorkItem.mTarget.mIdentifier = "ARRAY_ARRAY";
			newWorkItem.mPath = targetPath;
			workItems.emplace_back( rftl::move( newWorkItem ) );
			continue;
		}
		else if( rftl::holds_alternative<SquirrelVM::InstanceTag>( elemValue ) )
		{
			// Instance

			if( accessor->mInsertVariableDefault != nullptr )
			{
				// Insert a placeholder

				bool const writeSuccess = accessor->mInsertVariableDefault( location, &key, keyInfo );
				if( writeSuccess == false )
				{
					RF_TODO_ANNOTATION( "Try fallback to insert via unique pointer?" );
					RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
						"Failed to write placeholder value to accessor target" );
					return false;
				}
			}
			else if( accessor->mInsertVariableViaUPtr != nullptr )
			{
				// Construct a placeholder instance and insert it

				rftl::string const instanceClassName = vm.GetNestedInstanceClassName( targetPath );
				if( instanceClassName.empty() )
				{
					RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
						"Failed to determine class name for instance to construct" );
					return false;
				}

				rftype::ConstructedType constructed = ConstructClassByClassName( scratchLookup, instanceClassName );
				if( constructed.mLocation == nullptr )
				{
					RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
						"Failed to construct instance" );
					return false;
				}
				RF_ASSERT( constructed.mClassInfo != nullptr );

				// Copy the key into a UniquePtr to transfer it
				static_assert( rftl::is_same<decltype( key ), size_t const>::value );
				UniquePtr<size_t> keyCopy = DefaultCreator<size_t>::Create( key );

				reflect::VariableTypeInfo valueInfo = {};
				valueInfo.mClassInfo = constructed.mClassInfo;

				bool const writeSuccess =
					accessor->mInsertVariableViaUPtr(
						location,
						rftl::move( keyCopy ),
						keyInfo,
						rftl::move( constructed.mLocation ),
						valueInfo );
				if( writeSuccess == false )
				{
					RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
						"Failed to transfer placeholder value to accessor target" );
					return false;
				}
			}
			else
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to write any placeholder values to accessor target" );
				return false;
			}

			// Get the placeholder back out of the accessor to fill it out
			// NOTE: In the complex case where a derived class is being stored
			//  into a base class, this assumes that the accessor exposes
			//  derived class, if it supports virtual reflection lookup
			void const* arrayItemLoc = nullptr;
			reflect::VariableTypeInfo arrayItemInfo = {};
			bool const readSuccess = accessor->mGetTargetByKey( location, &key, keyInfo, arrayItemLoc, arrayItemInfo );
			if( readSuccess == false )
			{
				RFLOG_NOTIFY( targetPath, RFCAT_GAMESCRIPTING,
					"Failed to read placeholder value from accessor target" );
				return false;
			}

			// Queue population as later work
			WorkItem newWorkItem;
			newWorkItem.mTarget.mTypeInfo = arrayItemInfo;
			newWorkItem.mTarget.mLocation = arrayItemLoc;
			newWorkItem.mTarget.mIdentifier = "ARRAY_ITEM";
			newWorkItem.mPath = targetPath;
			workItems.emplace_back( rftl::move( newWorkItem ) );
			continue;
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
	ScratchLookup const& scratchLookup,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	TargetItem const& target )
{
	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = target.mTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
			"Failed to convert script array to member variable type" );
		return false;
	}
	void* const location = const_cast<void*>( target.mLocation );

	// Load the script values
	SquirrelVM::ElementArray const elemArr = vm.GetNestedVariableAsArray( currentPath );

	return ProcessElementArrayPopulationWork( vm, scratchLookup, workItems, currentPath, elemArr, accessor, location );
}



bool QueueScriptInstancePopulationWork(
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	TargetItem const& target )
{
	reflect::VariableTypeInfo const& targetTypeInfo = target.mTypeInfo;
	void const* targetLocation = target.mLocation;

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
		newWorkItem.mTarget.mTypeInfo = targetTypeInfo;
		newWorkItem.mTarget.mLocation = targetLocation;
		newWorkItem.mTarget.mIdentifier = "INSTANCE";
		newWorkItem.mPath = currentPath;
		workItems.emplace_back( rftl::move( newWorkItem ) );
		return true;
	}
	else
	{
		// Unknown

		RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
			"Failed to determine handling for script element instance into member variable type" );
		return false;
	}
}



bool ProcessScriptVariable(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	SquirrelVM::Element const& elemValue,
	TargetItem const& target )
{
	// TODO: Better diagnostic logic
	rftl::string_view const diagnosticName = currentPath.back().mIdentifier;

	if( rftl::holds_alternative<reflect::Value>( elemValue ) )
	{
		// Value
		bool const writeSuccess = WriteScriptValueToVariable( elemValue, target );
		if( writeSuccess == false )
		{
			RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Failed to write script value '{}' to variable",
				diagnosticName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::String>( elemValue ) )
	{
		// String
		bool const writeSuccess = WriteScriptStringToVariable( elemValue, target );
		if( writeSuccess == false )
		{
			RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Failed to write script string '{}' to variable",
				diagnosticName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::ArrayTag>( elemValue ) )
	{
		// Array
		bool const processSuccess = ProcessScriptArrayPopulationWork( vm, scratchLookup, workItems, currentPath, target );
		if( processSuccess == false )
		{
			RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Failed to process script array '{}' to variable",
				diagnosticName );
			return false;
		}
	}
	else if( rftl::holds_alternative<SquirrelVM::InstanceTag>( elemValue ) )
	{
		// Instance
		bool const queueSuccess = QueueScriptInstancePopulationWork( workItems, currentPath, target );
		if( queueSuccess == false )
		{
			RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Failed to prepare processing for script instance '{}' to be written into variable",
				diagnosticName );
			return false;
		}
	}
	else
	{
		RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
			"Unknown element type on '{}', probably an error in script-handling code",
			diagnosticName );
		return false;
	}

	return true;
}



bool ProcessClassWorkItem(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	reflect::ClassInfo const& classInfo,
	void const* classInstance )
{
	// Locally cache the class info from reflection
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers( classInfo, classInstance );

	// Extract the instance from script
	SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( currentPath );
	if( elemMap.empty() )
	{
		RFLOG_WARNING( currentPath, RFCAT_GAMESCRIPTING,
			"Element map is empty, assuming failure" );
		return false;
	}

	// For each element...
	for( SquirrelVM::ElementMap::value_type const& elemPair : elemMap )
	{
		SquirrelVM::Element const& elemValue = elemPair.second;

		// Get the name
		SquirrelVM::String const* elemString = rftl::get_if<SquirrelVM::String>( &( elemPair.first ) );
		char const* const elemName = elemString != nullptr ? elemString->c_str() : nullptr;
		if( elemName == nullptr )
		{
			RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Unable to determine identifier for variable" );
			continue;
		}

		// Check if it's the reserved class name member
		if( strcmp( elemName, SquirrelVM::kReservedClassNameMemberName ) == 0 )
		{
			if( rftl::holds_alternative<SquirrelVM::String>( elemValue ) == false )
			{
				RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
					"Reserved class name member is not a string" );
				continue;
			}
			SquirrelVM::String const& className = rftl::get<SquirrelVM::String>( elemValue );

			// Lookup the class info
			reflect::ClassInfo const* const foundClassInfo = FindClassInfoForName( scratchLookup, className );
			if( foundClassInfo == nullptr )
			{
				RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
					"Class claims to be '{}', but could not find corresponding injected type",
					className );
				continue;
			}

			if( foundClassInfo != &classInfo )
			{
				RFLOGF_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
					"Class claims to be '{}', injected type class info doesn't match",
					className );
				continue;
			}

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
			RFLOG_NOTIFY( currentPath, RFCAT_GAMESCRIPTING,
				"Unable to find matching variable with identifier" );
			continue;
		}
		rftype::TypeTraverser::MemberVariableInstance const& member = *foundMember;

		// Demote to a target, dropping member information
		TargetItem target = {};
		target.mTypeInfo = member.mMemberVariableInfo.mVariableTypeInfo;
		target.mLocation = member.mMemberVariableLocation;
		target.mIdentifier = member.mMemberVariableInfo.mIdentifier;

		// Handle the transfer from script->reflect
		SquirrelVM::NestedTraversalPath nestedPath = currentPath;
		nestedPath.emplace_back( elemName );
		bool const processSuccess = ProcessScriptVariable( vm, scratchLookup, workItems, nestedPath, elemValue, target );
		if( processSuccess == false )
		{
			// TODO: This should probably be conditional whether it
			//  ignores, or aborts the whole process
			continue;
		}
	}

	return true;
}



bool ProcessAccessorWorkItem(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	WorkItems& workItems,
	SquirrelVM::NestedTraversalPath const& currentPath,
	reflect::ExtensionAccessor const& accessor,
	void const* location )
{
	// Extract the instance from script
	SquirrelVM::Element const elemValue = vm.GetNestedVariable( currentPath );
	if( rftl::holds_alternative<reflect::Value>( elemValue ) && rftl::get<reflect::Value>( elemValue ).GetStoredType() == reflect::Value::Type::Invalid )
	{
		RFLOG_WARNING( currentPath, RFCAT_GAMESCRIPTING,
			"Element is invalid, assuming failure" );
		return false;
	}

	RF_ASSERT( rftl::holds_alternative<SquirrelVM::String>( elemValue ) || rftl::holds_alternative<SquirrelVM::ArrayTag>( elemValue ) );

	TargetItem target = {};
	target.mTypeInfo.mAccessor = &accessor;
	target.mLocation = location;
	target.mIdentifier = "ACCESSOR";

	// Handle the transfer from script->reflect
	bool const processSuccess = ProcessScriptVariable( vm, scratchLookup, workItems, currentPath, elemValue, target );
	if( processSuccess == false )
	{
		// TODO: This should probably be conditional whether it
		//  ignores, or aborts the whole process
		return false;
	}

	return true;
}



bool ProcessWorkItem(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	WorkItem const& currentWorkItem,
	WorkItems& workItems )
{
	// Don't support direct values as work items, expect them to be handled
	//  directly when encountered by a more complex work item
	RF_ASSERT( currentWorkItem.mTarget.mTypeInfo.mValueType == reflect::Value::Type::Invalid );

	// Class
	if( currentWorkItem.mTarget.mTypeInfo.mClassInfo != nullptr )
	{
		return ProcessClassWorkItem( vm, scratchLookup, workItems, currentWorkItem.mPath, *currentWorkItem.mTarget.mTypeInfo.mClassInfo, currentWorkItem.mTarget.mLocation );
	}

	// Accessor
	if( currentWorkItem.mTarget.mTypeInfo.mAccessor != nullptr )
	{
		return ProcessAccessorWorkItem( vm, scratchLookup, workItems, currentWorkItem.mPath, *currentWorkItem.mTarget.mTypeInfo.mAccessor, currentWorkItem.mTarget.mLocation );
	}

	RF_TODO_BREAK_MSG( "New type of work item?" );

	return false;
}



bool PopulateClassFromScript(
	SquirrelVM& vm,
	ScratchLookup const& scratchLookup,
	SquirrelVM::NestedTraversalPath scriptPath,
	reflect::ClassInfo const& classInfo,
	void* classInstance )
{
	rftl::vector<WorkItem> workItems;

	// Seed with the root class instance
	{
		WorkItem rootWorkItem = {};
		rootWorkItem.mPath = scriptPath;
		rootWorkItem.mTarget.mTypeInfo.mClassInfo = &classInfo;
		rootWorkItem.mTarget.mLocation = classInstance;
		rootWorkItem.mTarget.mIdentifier = "ROOT";
		workItems.emplace_back( rftl::move( rootWorkItem ) );
	}

	// While there is still work...
	while( workItems.empty() == false )
	{
		// Grab a work item
		WorkItem const workItem = workItems.back();
		workItems.pop_back();

		bool const processSuccess = ProcessWorkItem( vm, scratchLookup, workItem, workItems );
		if( processSuccess == false )
		{
			// TODO: This should probably be conditional whether it
			//  ignores, or aborts the whole process
			RFLOG_NOTIFY( scriptPath, RFCAT_GAMESCRIPTING,
				"Unable to convert part of a script" );
			if constexpr( true )
			{
				return false;
			}
			else
			{
				continue;
			}
		}
	}

	return true;
}

}
///////////////////////////////////////////////////////////////////////////////

OOLoader::OOLoader() = default;
OOLoader::~OOLoader() = default;



bool OOLoader::AllowTypeConstruction( TypeConstructor&& typeConstructor )
{
	if( typeConstructor == nullptr )
	{
		RF_DBGFAIL_MSG( "Null constructor" );
		return false;
	}

	if( mTypeConstructor != nullptr )
	{
		RF_DBGFAIL_MSG( "Cannot change constructor" );
		return false;
	}

	mTypeConstructor = rftl::move( typeConstructor );
	return true;
}



bool OOLoader::InjectReflectedClassByClassInfo( reflect::ClassInfo const& classInfo, rftl::string_view name )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = details::GetAllMembers( classInfo, nullptr );
	rftl::vector<rftl::string_view> const memberNames = details::GetAllMemberNames( members );
	bool const success = mVm.InjectSimpleStruct( name, memberNames );
	if( success )
	{
		mInjectedClasses.emplace_back( InjectedClass{ &classInfo, RFTLE_VSTR( name ) } );
		RF_ASSERT( mInjectedClasses.back().mName.empty() == false );
	}
	return success;
}



bool OOLoader::AddSourceFromBuffer( rftl::string_view buffer )
{
	return mVm.AddSourceFromBuffer( buffer );
}



bool OOLoader::PopulateClass( rftl::string_view rootVariableName, reflect::ClassInfo const& classInfo, void* classInstance )
{
	return PopulateClass( SquirrelVM::NestedTraversalPath{ rootVariableName }, classInfo, classInstance );
}



bool OOLoader::PopulateClass( SquirrelVM::NestedTraversalPath scriptPath, reflect::ClassInfo const& classInfo, void* classInstance )
{
	details::ScratchLookup const scratchLookup{ mInjectedClasses, mTypeConstructor };
	return details::PopulateClassFromScript( mVm, scratchLookup, scriptPath, classInfo, classInstance );
}

///////////////////////////////////////////////////////////////////////////////
}
