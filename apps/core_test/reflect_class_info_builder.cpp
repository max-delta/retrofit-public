#include "stdafx.h"

#include "core_reflect/ClassInfoBuilder.h"
#include "core/macros.h"


namespace RF::reflect {
///////////////////////////////////////////////////////////////////////////////

namespace details {

// NOTE: Static member variables not allowed in function-scope classes
struct ClassWithStaticMemberVariables
{
	static double static_d;
	static double const static_cd;
	static thread_local int16_t thread_i;
	static thread_local int16_t const thread_ci;
	static constexpr uint8_t constexpr_u = 0;
};
double ClassWithStaticMemberVariables::static_d;
double const ClassWithStaticMemberVariables::static_cd = 0.0;
thread_local int16_t ClassWithStaticMemberVariables::thread_i;
thread_local int16_t const ClassWithStaticMemberVariables::thread_ci = 0;

}



TEST( ReflectBuilder, ClassInfos )
{
	// Virtual table changes alignment on some compilers, such as with a
	//  pointer to the table stored in the instances
	constexpr size_t kAlignmentConstraintOfPolymorphism = sizeof( void* );

	{
		// Empty
		struct Test
		{
			//
		};
		ClassInfo classInfo{ ExplicitDefaultConstruct() };
		builder::CreateClassInfo<Test>( classInfo );
		ASSERT_TRUE( classInfo.mIsPolymorphic == false );
		ASSERT_TRUE( classInfo.mIsAbstract == false );
		ASSERT_TRUE( classInfo.mIsDefaultConstructible == true );
		ASSERT_TRUE( classInfo.mIsCopyConstructible == true );
		ASSERT_TRUE( classInfo.mIsMoveConstructible == true );
		ASSERT_TRUE( classInfo.mIsDestructible == true );
		ASSERT_TRUE( classInfo.mHasVirtualDestructor == false );
		ASSERT_TRUE( classInfo.mMinimumAlignment == 1 );
	}
	{
		// Virtual
		struct Test
		{
			virtual void Virt()
			{
			}
			RF_MSVC_INLINE_SUPPRESS( 5204 ); // No virtual destructor
		};
		ClassInfo classInfo{ ExplicitDefaultConstruct() };
		builder::CreateClassInfo<Test>( classInfo );
		ASSERT_TRUE( classInfo.mIsPolymorphic == true );
		ASSERT_TRUE( classInfo.mIsAbstract == false );
		ASSERT_TRUE( classInfo.mIsDefaultConstructible == true );
		ASSERT_TRUE( classInfo.mIsCopyConstructible == true );
		ASSERT_TRUE( classInfo.mIsMoveConstructible == true );
		ASSERT_TRUE( classInfo.mIsDestructible == true );
		ASSERT_TRUE( classInfo.mHasVirtualDestructor == false );
		ASSERT_TRUE( classInfo.mMinimumAlignment == kAlignmentConstraintOfPolymorphism );
	}
	{
		// Abstract
		struct Test
		{
			virtual void Virt() = 0;
			RF_MSVC_INLINE_SUPPRESS( 5204 ); // No virtual destructor
		};
		ClassInfo classInfo{ ExplicitDefaultConstruct() };
		builder::CreateClassInfo<Test>( classInfo );
		ASSERT_TRUE( classInfo.mIsPolymorphic == true );
		ASSERT_TRUE( classInfo.mIsAbstract == true );
		ASSERT_TRUE( classInfo.mIsDefaultConstructible == false );
		ASSERT_TRUE( classInfo.mIsCopyConstructible == false );
		ASSERT_TRUE( classInfo.mIsMoveConstructible == false );
		ASSERT_TRUE( classInfo.mIsDestructible == false );
		ASSERT_TRUE( classInfo.mHasVirtualDestructor == false );
		ASSERT_TRUE( classInfo.mMinimumAlignment == kAlignmentConstraintOfPolymorphism );
	}
	{
		// Correct destructor
		struct Test
		{
			virtual void Virt()
			{
			}
			virtual ~Test() = default;
		};
		ClassInfo classInfo{ ExplicitDefaultConstruct() };
		builder::CreateClassInfo<Test>( classInfo );
		ASSERT_TRUE( classInfo.mIsPolymorphic == true );
		ASSERT_TRUE( classInfo.mIsAbstract == false );
		ASSERT_TRUE( classInfo.mIsDefaultConstructible == true );
		ASSERT_TRUE( classInfo.mIsCopyConstructible == true );
		ASSERT_TRUE( classInfo.mIsMoveConstructible == true );
		ASSERT_TRUE( classInfo.mIsDestructible == true );
		ASSERT_TRUE( classInfo.mHasVirtualDestructor == true );
		ASSERT_TRUE( classInfo.mMinimumAlignment == kAlignmentConstraintOfPolymorphism );
	}
	{
		// Deleted functionality
		struct Test
		{
			Test() = delete;
			Test( Test const& ) = delete;
			Test( Test&& ) = delete;
			~Test() = delete;
			Test& operator=( Test const& ) = delete;
			Test& operator=( Test const&& ) = delete;
		};
		ClassInfo classInfo{ ExplicitDefaultConstruct() };
		builder::CreateClassInfo<Test>( classInfo );
		ASSERT_TRUE( classInfo.mIsPolymorphic == false );
		ASSERT_TRUE( classInfo.mIsAbstract == false );
		ASSERT_TRUE( classInfo.mIsDefaultConstructible == false );
		ASSERT_TRUE( classInfo.mIsCopyConstructible == false );
		ASSERT_TRUE( classInfo.mIsMoveConstructible == false );
		ASSERT_TRUE( classInfo.mIsDestructible == false );
		ASSERT_TRUE( classInfo.mHasVirtualDestructor == false );
		ASSERT_TRUE( classInfo.mMinimumAlignment == 1 );
	}
}



TEST( ReflectBuilder, FreeStandingVariableInfos )
{
	using Test = details::ClassWithStaticMemberVariables;

	{
		FreeStandingVariableInfo varInfo;
		builder::CreateFreeStandingVariableInfo( varInfo, &Test::static_d );
		ASSERT_TRUE( varInfo.mAddress == &Test::static_d );
		ASSERT_TRUE( varInfo.mMutable == true );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::static_d ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Double );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
	{
		FreeStandingVariableInfo varInfo;
		builder::CreateFreeStandingVariableInfo( varInfo, &Test::static_cd );
		ASSERT_TRUE( varInfo.mAddress == &Test::static_cd );
		ASSERT_TRUE( varInfo.mMutable == false );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::static_cd ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Double );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
	{
		FreeStandingVariableInfo varInfo;
		builder::CreateFreeStandingVariableInfo( varInfo, &Test::thread_i );
		ASSERT_TRUE( varInfo.mAddress == &Test::thread_i );
		ASSERT_TRUE( varInfo.mMutable == true );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::thread_i ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Int16 );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
	{
		FreeStandingVariableInfo varInfo;
		builder::CreateFreeStandingVariableInfo( varInfo, &Test::thread_ci );
		ASSERT_TRUE( varInfo.mAddress == &Test::thread_ci );
		ASSERT_TRUE( varInfo.mMutable == false );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::thread_ci ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Int16 );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
	{
		FreeStandingVariableInfo varInfo;
		builder::CreateFreeStandingVariableInfo( varInfo, &Test::constexpr_u );
		ASSERT_TRUE( varInfo.mAddress == &Test::constexpr_u );
		ASSERT_TRUE( varInfo.mMutable == false );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::constexpr_u ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::UInt8 );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
}



TEST( ReflectBuilder, MemberVariableInfos )
{
	struct Test
	{
		RF_NO_COPY( Test );
		Test() = delete;
		char unusedPadding[16];
		float instance_f;
		float const instance_cf;
	};

	{
		MemberVariableInfo varInfo;
		builder::CreateMemberVariableInfo( varInfo, &Test::instance_f );
		ASSERT_TRUE( varInfo.mOffset == offsetof( Test, instance_f ) );
		ASSERT_TRUE( varInfo.mMutable == true );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::instance_f ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Float );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
	{
		MemberVariableInfo varInfo;
		builder::CreateMemberVariableInfo( varInfo, &Test::instance_cf );
		ASSERT_TRUE( varInfo.mOffset == offsetof( Test, instance_cf ) );
		ASSERT_TRUE( varInfo.mMutable == false );
		ASSERT_TRUE( varInfo.mSize == sizeof( Test::instance_cf ) );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mValueType == Value::Type::Float );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mClassInfo == nullptr );
		ASSERT_TRUE( varInfo.mVariableTypeInfo.mAccessor == nullptr );
	}
}



TEST( ReflectBuilder, FreeStandingFunctionInfos )
{
	struct Test
	{
		static void v_call_v( void )
		{
			return;
		}
		static float f_call_f( float )
		{
			return 0.f;
		}
		static double d_call_ifc( int, float, char )
		{
			return 0.;
		}
	};

	{
		FreeStandingFunctionInfo funcInfo;
		builder::CreateFreeStandingFunctionInfo( funcInfo, Test::v_call_v );
		ASSERT_TRUE( funcInfo.mAddress == Test::v_call_v );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Invalid );
		ASSERT_TRUE( funcInfo.Parameters.size() == 0 );
	}
	{
		FreeStandingFunctionInfo funcInfo;
		builder::CreateFreeStandingFunctionInfo( funcInfo, Test::f_call_f );
		ASSERT_TRUE( funcInfo.mAddress == Test::f_call_f );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters.size() == 1 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Float );
	}
	{
		FreeStandingFunctionInfo funcInfo;
		builder::CreateFreeStandingFunctionInfo( funcInfo, Test::d_call_ifc );
		ASSERT_TRUE( funcInfo.mAddress == Test::d_call_ifc );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Double );
		ASSERT_TRUE( funcInfo.Parameters.size() == 3 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Int32 );
		ASSERT_TRUE( funcInfo.Parameters[1].mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters[2].mValueType == Value::Type::Char );
	}
}



TEST( ReflectBuilder, NonConstMemberFunctionInfos )
{
	struct Test
	{
		void v_call_v( void )
		{
			return;
		}
		float f_call_f( float )
		{
			return 0.f;
		}
		double d_call_ifc( int, float, char )
		{
			return 0.;
		}
	};

	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::v_call_v );
		ASSERT_TRUE( funcInfo.mRequiresConst == false );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Invalid );
		ASSERT_TRUE( funcInfo.Parameters.size() == 0 );
	}
	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::f_call_f );
		ASSERT_TRUE( funcInfo.mRequiresConst == false );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters.size() == 1 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Float );
	}
	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::d_call_ifc );
		ASSERT_TRUE( funcInfo.mRequiresConst == false );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Double );
		ASSERT_TRUE( funcInfo.Parameters.size() == 3 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Int32 );
		ASSERT_TRUE( funcInfo.Parameters[1].mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters[2].mValueType == Value::Type::Char );
	}
}



TEST( ReflectBuilder, ConstMemberFunctionInfos )
{
	struct Test
	{
		void v_call_v( void ) const
		{
			return;
		}
		float f_call_f( float ) const
		{
			return 0.f;
		}
		double d_call_ifc( int, float, char ) const
		{
			return 0.;
		}
	};

	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::v_call_v );
		ASSERT_TRUE( funcInfo.mRequiresConst == true );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Invalid );
		ASSERT_TRUE( funcInfo.Parameters.size() == 0 );
	}
	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::f_call_f );
		ASSERT_TRUE( funcInfo.mRequiresConst == true );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters.size() == 1 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Float );
	}
	{
		MemberFunctionInfo funcInfo;
		builder::CreateMemberFunctionInfo( funcInfo, &Test::d_call_ifc );
		ASSERT_TRUE( funcInfo.mRequiresConst == true );
		ASSERT_TRUE( funcInfo.mReturn.mValueType == Value::Type::Double );
		ASSERT_TRUE( funcInfo.Parameters.size() == 3 );
		ASSERT_TRUE( funcInfo.Parameters[0].mValueType == Value::Type::Int32 );
		ASSERT_TRUE( funcInfo.Parameters[1].mValueType == Value::Type::Float );
		ASSERT_TRUE( funcInfo.Parameters[2].mValueType == Value::Type::Char );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
