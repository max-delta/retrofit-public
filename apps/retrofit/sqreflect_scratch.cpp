#include "stdafx.h"

#include "Scripting_squirrel/squirrel.h"
#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/TypeTraverser.h"
//#include "core_rftype/stl_extensions/vector.h"


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
	//RFTYPE_META().ExtensionProperty( "mString", &SQReflectTestClass::mString );
	//RFTYPE_META().ExtensionProperty( "mWString", &SQReflectTestClass::mWString );
	//RFTYPE_META().ExtensionProperty( "mIntArray", &SQReflectTestClass::mIntArray );
	//RFTYPE_META().RawProperty( "mNested", &SQReflectTestClass::mNested );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestClass" );
}


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////

template<typename ReflectedClass>
rftl::vector<rftype::TypeTraverser::MemberVariableInstance> GetAllMembers( ReflectedClass const* classInstance )
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> members;

	auto onMemberVariableFunc = [&members](
		rftype::TypeTraverser::MemberVariableInstance const& varInst ) ->
		void
	{
		members.emplace_back( varInst );
	};

	auto onNestedTypeFoundFunc = [&members](
		rftype::TypeTraverser::MemberVariableInstance const& varInst,
		bool& shouldRecurse ) ->
		void
	{
		members.emplace_back( varInst );
		shouldRecurse = false;
	};

	auto onReturnFromNestedTypeFunc = [](
		void ) ->
		void
	{
		RF_DBGFAIL_MSG( "Unexpected recursion" );
	};

	rftype::TypeTraverser::TraverseVariablesT(
		rftype::GetClassInfo<ReflectedClass>(),
		classInstance,
		onMemberVariableFunc,
		onNestedTypeFoundFunc,
		onReturnFromNestedTypeFunc );

	return members;
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
bool InjectReflectedClassByCompileType( script::SquirrelVM& vm, char const* name)
{
	rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers<ReflectedClass>();
	rftl::vector<char const*> const memberNames = GetAllMemberNames( members );
	return vm.InjectSimpleStruct( name, memberNames.data(), memberNames.size() );
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
			"//x.mString = \"test\";\n"
			"//x.mWString = \"test\";\n"
			"//x.mIntArray = [3,5,7];\n"
			"//x.mNested = SQReflectTestNestedClass();\n"
			"//x.mNested.mBool = true;\n"
			"\n";
		bool const sourceAdd = vm.AddSourceFromBuffer( source );
		RF_ASSERT( sourceAdd );
	}

	// Extract
	script::SquirrelVM::ElementMap const elemMap = vm.GetGlobalVariableAsInstance( "x" );
	RF_ASSERT( elemMap.size() == 18 );

	// Instantiate
	SQReflectTestClass testClass = {};

	// Populate
	{
		for( script::SquirrelVM::ElementMap::value_type const& elemPair : elemMap )
		{
			script::SquirrelVM::String const* elemString = rftl::get_if<script::SquirrelVM::String>( &( elemPair.first ) );
			char const* const elemName = elemString != nullptr ? elemString->c_str() : nullptr;
			if( elemName == nullptr )
			{
				RF_DBGFAIL_MSG( "Unable to determine identifier for variable" );
				continue;
			}

			rftl::vector<rftype::TypeTraverser::MemberVariableInstance> const members = GetAllMembers<SQReflectTestClass>( &testClass );
			rftype::TypeTraverser::MemberVariableInstance const* foundMember = nullptr;
			for( rftype::TypeTraverser::MemberVariableInstance const& member : members )
			{
				char const* const memberName = member.mMemberVariableInfo.mIdentifier;
				if( strcmp( elemName, memberName ) == 0 )
				{
					RF_ASSERT(foundMember == nullptr);
					foundMember = &member;
				}
			}
			if( foundMember == nullptr )
			{
				RF_DBGFAIL_MSG( "Unable to find matching variable with identifier" );
				continue;
			}
			rftype::TypeTraverser::MemberVariableInstance const& member = *foundMember;

			script::SquirrelVM::Element const& elemValue = elemPair.second;
			if( rftl::holds_alternative<reflect::Value>( elemValue ) )
			{
				reflect::Value const& source = rftl::get<reflect::Value>( elemValue );
				void* const destination = const_cast<void*>( member.mMemberVariableLocation );

				reflect::Value::Type const destinationType = member.mMemberVariableInfo.mVariableTypeInfo.mValueType;
				reflect::Value const intermediate = source.ConvertTo( destinationType );
				if( intermediate.GetStoredType() != destinationType )
				{
					RF_DBGFAIL_MSG("Failed to convert script element to member variable type");
					continue;
				}

				void const* intermediateData = intermediate.GetBytes();
				size_t const intermediateSize = intermediate.GetNumBytes();
				memcpy( destination, intermediateData, intermediateSize );
			}
			else if( rftl::holds_alternative<script::SquirrelVM::String>( elemValue ) )
			{
				RF_DBGFAIL_MSG( "TODO" );
			}
			else if( rftl::holds_alternative<script::SquirrelVM::ArrayTag>( elemValue ) )
			{
				RF_DBGFAIL_MSG( "TODO" );
			}
			else if( rftl::holds_alternative<script::SquirrelVM::InstanceTag>( elemValue ) )
			{
				RF_DBGFAIL_MSG( "TODO" );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
}}
