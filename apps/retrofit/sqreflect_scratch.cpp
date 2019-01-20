#include "stdafx.h"

#include "Scripting_squirrel/squirrel.h"
#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/TypeTraverser.h"
#include "core_rftype/stl_extensions/vector.h"
#include "core_rftype/stl_extensions/string.h"

#include "core/meta/ScopedCleanup.h"


struct SQReflectTestNestedClass
{
	bool mBool;
};

struct SQReflectTestClass
{
	bool mBool;
	void* mVoidPtr;
	RF::reflect::VirtualClass* mClassPtr;
	char mChar;
	wchar_t mWChar;
	char16_t mChar16;
	char32_t mChar32;
	float mFloat;
	double mDouble;
	long double mLongDouble;
	uint8_t mU8;
	int8_t mS8;
	uint16_t mU16;
	int16_t mS16;
	uint32_t mU32;
	int32_t mS32;
	uint64_t mU64;
	int64_t mS64;
	std::string mString;
	std::wstring mWString;
	std::vector<int32_t> mIntArray;
	SQReflectTestNestedClass mNested;
};

RFTYPE_CREATE_META( SQReflectTestNestedClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestNestedClass::mBool );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestNestedClass" );
}

RFTYPE_CREATE_META( SQReflectTestClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestClass::mBool );
	RFTYPE_META().RawProperty( "mVoidPtr", &SQReflectTestClass::mVoidPtr );
	RFTYPE_META().RawProperty( "mClassPtr", &SQReflectTestClass::mClassPtr );
	RFTYPE_META().RawProperty( "mChar", &SQReflectTestClass::mChar );
	RFTYPE_META().RawProperty( "mWChar", &SQReflectTestClass::mWChar );
	RFTYPE_META().RawProperty( "mChar16", &SQReflectTestClass::mChar16 );
	RFTYPE_META().RawProperty( "mChar32", &SQReflectTestClass::mChar32 );
	RFTYPE_META().RawProperty( "mFloat", &SQReflectTestClass::mFloat );
	RFTYPE_META().RawProperty( "mDouble", &SQReflectTestClass::mDouble );
	RFTYPE_META().RawProperty( "mLongDouble", &SQReflectTestClass::mLongDouble );
	RFTYPE_META().RawProperty( "mU8", &SQReflectTestClass::mU8 );
	RFTYPE_META().RawProperty( "mS8", &SQReflectTestClass::mS8 );
	RFTYPE_META().RawProperty( "mU16", &SQReflectTestClass::mU16 );
	RFTYPE_META().RawProperty( "mS16", &SQReflectTestClass::mS16 );
	RFTYPE_META().RawProperty( "mU32", &SQReflectTestClass::mU32 );
	RFTYPE_META().RawProperty( "mS32", &SQReflectTestClass::mS32 );
	RFTYPE_META().RawProperty( "mU64", &SQReflectTestClass::mU64 );
	RFTYPE_META().RawProperty( "mS64", &SQReflectTestClass::mS64 );
	RFTYPE_META().ExtensionProperty( "mString", &SQReflectTestClass::mString );
	RFTYPE_META().ExtensionProperty( "mWString", &SQReflectTestClass::mWString );
	RFTYPE_META().ExtensionProperty( "mIntArray", &SQReflectTestClass::mIntArray );
	RFTYPE_META().RawProperty( "mNested", &SQReflectTestClass::mNested );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestClass" );
}


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////

struct WorkItem
{
	script::SquirrelVM::NestedTraversalPath mPath;
	reflect::ClassInfo const* mClassInfo;
	void const* mClassInstance;
};
using WorkItems = rftl::vector<WorkItem>;



rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers( reflect::ClassInfo const& classInfo, void const* classInstance )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> members;

	auto onMemberVariableFunc = [&members](
		rftype::TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	auto onTraversalTypeFoundFunc = [&members](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		shouldRecurse = false;
	};

	auto onReturnFromTraversalTypeFunc = [](
		rftype::TypeTraverser::TraversalType traversalType,
		rftype::TypeTraverser::TraversalVariableInstance const& varInst ) ->
		void
	{
		RF_DBGFAIL_MSG( "Unexpected recursion" );
	};

	rftype::TypeTraverser::TraverseVariablesT(
		classInfo,
		classInstance,
		onMemberVariableFunc,
		onTraversalTypeFoundFunc,
		onReturnFromTraversalTypeFunc );

	return members;
}



template<typename ReflectedClass>
rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers( ReflectedClass const* classInstance )
{
	return GetAllMembers( rftype::GetClassInfo<ReflectedClass>(), classInstance );
}



template<typename ReflectedClass>
rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers()
{
	return GetAllMembers<ReflectedClass>( nullptr );
}



rftl::vector<char const*> GetAllMemberNames( rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const& members )
{
	rftl::vector<char const*> memberNames;
	for( rftype::TypeTraverser::MemberVariableInstance const& member : members )
	{
		memberNames.emplace_back( member.mMemberVariableInfo.mIdentifier );
	}
	return memberNames;
}



template<typename ReflectedClass>
bool InjectReflectedClassByCompileType( script::SquirrelVM& vm, char const* name )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers<ReflectedClass>();
	rftl::vector<char const*> const memberNames = GetAllMemberNames( members );
	return vm.InjectSimpleStruct( name, memberNames.data(), memberNames.size() );
}



bool WriteScriptValueToMemberVariable( script::SquirrelVM::Element const& elemValue, rftype::TypeTraverser::MemberVariableInstance const& member )
{
	RF_ASSERT( rftl::holds_alternative<reflect::Value>( elemValue ) );

	// Load the script value
	reflect::Value const& source = rftl::get<reflect::Value>( elemValue );
	void* const destination = const_cast<void*>( member.mMemberVariableLocation );

	// Ensure it's compatible
	reflect::Value::Type const destinationType = member.mMemberVariableInfo.mVariableTypeInfo.mValueType;
	reflect::Value const intermediate = source.ConvertTo( destinationType );
	if( intermediate.GetStoredType() != destinationType )
	{
		RF_DBGFAIL_MSG( "Failed to convert script element to member variable type" );
		return false;
	}

	// Write to reflection
	void const* intermediateData = intermediate.GetBytes();
	size_t const intermediateSize = intermediate.GetNumBytes();
	memcpy( destination, intermediateData, intermediateSize );
	return true;
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
		RF_DBGFAIL_MSG( "Failed to convert script value to accessor target variable type" );
		return false;
	}

	reflect::Value const intermediate = source.ConvertTo( targetValueType );
	if( intermediate.GetStoredType() != targetValueType )
	{
		RF_DBGFAIL_MSG( "Failed to convert script value to accessor target value type" );
		return false;
	}

	void const* const writeableValue = intermediate.GetBytes();
	bool const writeSuccess = accessor->mInsertVariableViaCopy( location, &key, keyInfo, writeableValue, targetInfo );
	if( writeSuccess == false )
	{
		RF_DBGFAIL_MSG( "Failed to write script value to accessor target" );
		return false;
	}

	return true;
}



bool ProcessElementArrayPopulationWork(
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	script::SquirrelVM::ElementArray const& elemArr,
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

	// For each value...
	size_t indexCounter = 0;
	reflect::VariableTypeInfo keyInfo = {};
	keyInfo.mValueType = reflect::Value::DetermineType<size_t>();
	for( script::SquirrelVM::Element const& elemValue : elemArr )
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
				RF_DBGFAIL_MSG( "Failed to store script value in accessor" );
				return false;
			}
		}
		else if( rftl::holds_alternative<script::SquirrelVM::String>( elemValue ) )
		{
			// String

			// ???Recurse???
			RF_DBGFAIL_MSG( "TODO" );
			return false;
		}
		else if( rftl::holds_alternative<script::SquirrelVM::ArrayTag>( elemValue ) )
		{
			// Array

			// ???Recurse???
			RF_DBGFAIL_MSG( "TODO" );
			return false;
		}
		else if( rftl::holds_alternative<script::SquirrelVM::InstanceTag>( elemValue ) )
		{
			// Instance

			// Run instance - handler code
			// ???Queue???
			RF_DBGFAIL_MSG( "TODO" );
			return false;
		}
		else
		{
			RF_DBGFAIL_MSG( "TODO" );
			return false;
		}
	}

	return true;
}



bool WriteScriptStringToMemberVariable( script::SquirrelVM::Element const& elemValue, rftype::TypeTraverser::MemberVariableInstance const& member )
{
	RF_ASSERT( rftl::holds_alternative<script::SquirrelVM::String>( elemValue ) );
	script::SquirrelVM::String const& string = rftl::get<script::SquirrelVM::String>( elemValue );

	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = member.mMemberVariableInfo.mVariableTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RF_DBGFAIL_MSG( "Failed to convert script array to member variable type" );
		return false;
	}
	void* const location = const_cast<void*>( member.mMemberVariableLocation );

	// Scoped mutation
	if( accessor->mBeginMutation != nullptr )
	{
		accessor->mBeginMutation( location );
	}
	auto endMutation = OnScopeEnd( [accessor, location]() {
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
			RF_DBGFAIL_MSG( "Failed to store script character in accessor" );
			return false;
		}
	}

	return true;
}



bool ProcessScriptArrayPopulationWork(
	script::SquirrelVM& vm,
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	char const* const elemName,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	// Ensure it's even remotely compatible
	reflect::ExtensionAccessor const* accessor = member.mMemberVariableInfo.mVariableTypeInfo.mAccessor;
	if( accessor == nullptr )
	{
		RF_DBGFAIL_MSG( "Failed to convert script array to member variable type" );
		return false;
	}
	void* const location = const_cast<void*>( member.mMemberVariableLocation );

	// Load the script values
	script::SquirrelVM::NestedTraversalPath path = currentWorkItem.mPath;
	path.emplace_back( elemName );
	script::SquirrelVM::ElementArray const elemArr = vm.GetNestedVariableAsArray( path );

	return ProcessElementArrayPopulationWork( currentWorkItem, workItems, elemArr, accessor, location );
}



bool QueueScriptInstancePopulationWork( WorkItem const& currentWorkItem, WorkItems& workItems, char const* const elemName, rftype::TypeTraverser::MemberVariableInstance const& member )
{
	reflect::VariableTypeInfo const& typeInfo = member.mMemberVariableInfo.mVariableTypeInfo;
	if( typeInfo.mAccessor != nullptr )
	{
		// Accessor

		// Instantiate
		RF_DBGFAIL_MSG( "TODO" );

		// Queue population as later work
		RF_DBGFAIL_MSG( "TODO" );
		return true;
	}
	else if( typeInfo.mClassInfo != nullptr )
	{
		// Direct nesting

		// Queue population as later work
		WorkItem newWorkItem;
		newWorkItem.mClassInfo = typeInfo.mClassInfo;
		newWorkItem.mClassInstance = member.mMemberVariableLocation;
		newWorkItem.mPath = currentWorkItem.mPath;
		newWorkItem.mPath.emplace_back( elemName );
		workItems.emplace_back( rftl::move( newWorkItem ) );
		return true;
	}
	else
	{
		// Unknown

		RF_DBGFAIL_MSG( "Failed to determine handling for script element instance into member variable type" );
		return false;
	}
}



bool ProcessScriptVariable(
	script::SquirrelVM& vm,
	WorkItem const& currentWorkItem,
	WorkItems& workItems,
	char const* const elemName,
	script::SquirrelVM::Element const& elemValue,
	rftype::TypeTraverser::MemberVariableInstance const& member )
{
	if( rftl::holds_alternative<reflect::Value>( elemValue ) )
	{
		// Value
		bool const writeSuccess = WriteScriptValueToMemberVariable( elemValue, member );
		if( writeSuccess == false )
		{
			RF_DBGFAIL_MSG( "Failed to write script value to member variable" );
			return false;
		}
	}
	else if( rftl::holds_alternative<script::SquirrelVM::String>( elemValue ) )
	{
		// String
		bool const writeSuccess = WriteScriptStringToMemberVariable( elemValue, member );
		if( writeSuccess == false )
		{
			RF_DBGFAIL_MSG( "Failed to write script string to member variable" );
			return false;
		}
	}
	else if( rftl::holds_alternative<script::SquirrelVM::ArrayTag>( elemValue ) )
	{
		// Array
		bool const processSuccess = ProcessScriptArrayPopulationWork( vm, currentWorkItem, workItems, elemName, member );
		if( processSuccess == false )
		{
			RF_DBGFAIL_MSG( "Failed to process script array to member variable" );
			return false;
		}
	}
	else if( rftl::holds_alternative<script::SquirrelVM::InstanceTag>( elemValue ) )
	{
		// Instance
		bool const queueSuccess = QueueScriptInstancePopulationWork( currentWorkItem, workItems, elemName, member );
		if( queueSuccess == false )
		{
			RF_DBGFAIL_MSG( "Failed to prepare processing for script instance to be written into member variable" );
			return false;
		}
	}
	else
	{
		RF_DBGFAIL_MSG( "Unknown element type, probably an error in script-handling code" );
		return false;
	}

	return true;
}



void sqreflect_scratch()
{
	script::SquirrelVM vm;

	// Inject
	{
		bool const inject = InjectReflectedClassByCompileType<SQReflectTestClass>( vm, "SQReflectTestClass" );
		RF_ASSERT( inject );
	}
	{
		bool const inject = InjectReflectedClassByCompileType<SQReflectTestNestedClass>( vm, "SQReflectTestNestedClass" );
		RF_ASSERT( inject );
	}

	// Use
	{
		constexpr char source[] =
			"x <- SQReflectTestClass();\n"
			"x.mBool = true;\n"
			"x.mVoidPtr = null;\n"
			"x.mClassPtr = null;\n"
			"x.mChar = 'a';\n"
			"x.mWChar = 'a';\n"
			"x.mChar16 = 'a';\n"
			"x.mChar32 = 'a';\n"
			"x.mFloat = 0.5;\n"
			"x.mDouble = 0.5;\n"
			"x.mLongDouble = 0.5;\n"
			"x.mU8 = 7;\n"
			"x.mS8 = 7;\n"
			"x.mU16 = 7;\n"
			"x.mS16 = 7;\n"
			"x.mU32 = 7;\n"
			"x.mS32 = 7;\n"
			"x.mU64 = 7;\n"
			"x.mS64 = 7;\n"
			"x.mString = \"test\";\n"
			"x.mWString = \"test\";\n"
			"x.mIntArray = [3,5,7];\n"
			"x.mNested = SQReflectTestNestedClass();\n"
			"x.mNested.mBool = true;\n"
			"\n";
		bool const sourceAdd = vm.AddSourceFromBuffer( source );
		RF_ASSERT( sourceAdd );
	}

	// Instantiate
	SQReflectTestClass testClass = {};
	reflect::ClassInfo const& testClassInfo = rftype::GetClassInfo<SQReflectTestClass>();

	// Populate
	{
		rftl::vector<WorkItem> workItems;

		workItems.push_back(
			WorkItem{
				{ "x" },
				&testClassInfo,
				&testClass } );

		// While there is still work...
		while( workItems.empty() == false )
		{
			// Grab a work item
			WorkItem const workItem = workItems.back();
			workItems.pop_back();

			script::SquirrelVM::NestedTraversalPath const& path = workItem.mPath;
			reflect::ClassInfo const& classInfo = *workItem.mClassInfo;
			void const* classInstance = workItem.mClassInstance;

			// Locally cache the class info from reflection
			rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers( classInfo, classInstance );

			// Extract the instance from script
			script::SquirrelVM::ElementMap const elemMap = vm.GetNestedVariableAsInstance( path );
			RF_ASSERT( elemMap.size() > 0 ); // HACK: For testing purposes only

			// For each element...
			for( script::SquirrelVM::ElementMap::value_type const& elemPair : elemMap )
			{
				// Get the name
				script::SquirrelVM::String const* elemString = rftl::get_if<script::SquirrelVM::String>( &( elemPair.first ) );
				char const* const elemName = elemString != nullptr ? elemString->c_str() : nullptr;
				if( elemName == nullptr )
				{
					RF_DBGFAIL_MSG( "Unable to determine identifier for variable" );
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
					RF_DBGFAIL_MSG( "Unable to find matching variable with identifier" );
					continue;
				}
				rftype::TypeTraverser::MemberVariableInstance const& member = *foundMember;

				// Handle the transfer from script->reflect
				script::SquirrelVM::Element const& elemValue = elemPair.second;
				bool const processSuccess = ProcessScriptVariable( vm, workItem, workItems, elemName, elemValue, member );
				if( processSuccess == false )
				{
					// TODO: This should probably be conditional whether it
					//  ignores, or aborts the whole process
					continue;
				}
			}
		}
	}

	// Verify
	RF_ASSERT( testClass.mBool == true );
	RF_ASSERT( testClass.mVoidPtr == nullptr );
	RF_ASSERT( testClass.mClassPtr == nullptr );
	RF_ASSERT( testClass.mChar == 'a' );
	RF_ASSERT( testClass.mWChar == L'a' );
	RF_ASSERT( testClass.mChar16 == u'a' );
	RF_ASSERT( testClass.mChar32 == U'a' );
	RF_ASSERT( testClass.mFloat == 0.5f );
	RF_ASSERT( testClass.mDouble == 0.5 );
	RF_ASSERT( testClass.mLongDouble == 0.5l );
	RF_ASSERT( testClass.mU8 == 7 );
	RF_ASSERT( testClass.mS8 == 7 );
	RF_ASSERT( testClass.mU16 == 7 );
	RF_ASSERT( testClass.mS16 == 7 );
	RF_ASSERT( testClass.mU32 == 7 );
	RF_ASSERT( testClass.mS32 == 7 );
	RF_ASSERT( testClass.mU64 == 7 );
	RF_ASSERT( testClass.mS64 == 7 );
	RF_ASSERT( testClass.mString == "test" );
	RF_ASSERT( testClass.mWString == L"test" );
	RF_ASSERT( testClass.mIntArray.size() == 3 );
	RF_ASSERT( testClass.mIntArray[0] == 3 );
	RF_ASSERT( testClass.mIntArray[1] == 5 );
	RF_ASSERT( testClass.mIntArray[2] == 7 );
	RF_ASSERT( testClass.mNested.mBool == true );
}

///////////////////////////////////////////////////////////////////////////////
}}
