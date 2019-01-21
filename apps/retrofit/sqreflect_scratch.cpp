#include "stdafx.h"

#include "GameScripting/OOLoader.h"

#include "RFType/CreateClassInfoDefinition.h"

#include "core_rftype/stl_extensions/vector.h"
#include "core_rftype/stl_extensions/string.h"


struct SQReflectTestContainedClass
{
	bool mBool;
};

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
	std::vector<SQReflectTestContainedClass> mObjArray;
};

RFTYPE_CREATE_META( SQReflectTestContainedClass )
{
	RFTYPE_META().RawProperty( "mBool", &SQReflectTestContainedClass::mBool );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestContainedClass" );
}

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
	RFTYPE_META().ExtensionProperty( "mObjArray", &SQReflectTestClass::mObjArray );
	RFTYPE_META().RawProperty( "mNested", &SQReflectTestClass::mNested );
	RFTYPE_REGISTER_BY_NAME( "SQReflectTestClass" );
}


namespace RF { namespace scratch {
///////////////////////////////////////////////////////////////////////////////

void sqreflect_scratch()
{
	script::OOLoader loader;

	// Inject
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestClass>( "SQReflectTestClass" );
		RF_ASSERT( inject );
	}
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestNestedClass>( "SQReflectTestNestedClass" );
		RF_ASSERT( inject );
	}
	{
		bool const inject = loader.InjectReflectedClassByCompileType<SQReflectTestContainedClass>( "SQReflectTestContainedClass" );
		RF_ASSERT( inject );
	}

	// Load
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
			"x.mObjArray = [ SQReflectTestContainedClass() ];\n"
			"x.mObjArray[0].mBool = true;\n"
			"x.mNested = SQReflectTestNestedClass();\n"
			"x.mNested.mBool = true;\n"
			"\n";
		bool const sourceAdd = loader.AddSourceFromBuffer( source );
		RF_ASSERT( sourceAdd );
	}

	// Instantiate
	SQReflectTestClass testClass = {};

	// Populate
	bool const populateSuccess = loader.PopulateClass( "x", testClass );
	RF_ASSERT( populateSuccess );

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
	RF_ASSERT( testClass.mObjArray.size() == 1 );
	RF_ASSERT( testClass.mObjArray[0].mBool == true );
	RF_ASSERT( testClass.mNested.mBool == true );
}

///////////////////////////////////////////////////////////////////////////////
}}
