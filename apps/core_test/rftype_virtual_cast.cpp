#include "stdafx.h"

#include "core_rftype/VirtualCast.h"
#include "core_rftype/CreateClassInfoDefinition.h"

#include "core/ptr/unique_ptr.h"
#include "core/ptr/default_creator.h"


namespace RF::rftype {
///////////////////////////////////////////////////////////////////////////////

namespace details {

struct Parent : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t parentPad;
};

struct Child : public Parent
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t childPad;
};

struct Sibling : public Parent
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t siblingPad;
};

struct LeftParent : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t leftPad;
};
struct RightParent : public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t rightPad;
};
struct MultiChild : public LeftParent, RightParent
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t childPad;
};


RF_ACK_UNSAFE_INHERITANCE
struct VirtualTop : virtual public reflect::VirtualClass
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t topPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualLeft : virtual public VirtualTop
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t leftPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualRight : virtual public VirtualTop
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t rightPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualFarLeft : virtual public VirtualLeft
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t farLeftPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualFarRight : virtual public VirtualRight
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t farRightPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualDiamondCenter : virtual public VirtualLeft, virtual public VirtualRight
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t diamondCenterPad;
};

RF_ACK_UNSAFE_INHERITANCE
struct VirtualExtremeDiamondCenter : virtual public VirtualFarLeft, virtual public VirtualFarRight
{
	RFTYPE_ENABLE_VIRTUAL_LOOKUP();

	uint8_t extremeDiamondCenterPad;
};

}

//
////
//////
////////
} // Temp close of namespace, simulate .cpp file vs .h file
//////////

RFTYPE_CREATE_META( RF::rftype::details::Parent )
{
}
RFTYPE_CREATE_META( RF::rftype::details::Child )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().BaseClass<Parent>();
}
RFTYPE_CREATE_META( RF::rftype::details::Sibling )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().BaseClass<Parent>();
}

RFTYPE_CREATE_META( RF::rftype::details::LeftParent )
{
}
RFTYPE_CREATE_META( RF::rftype::details::RightParent )
{
}
RFTYPE_CREATE_META_FOR_DIAMONDS( RF::rftype::details::MultiChild )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<LeftParent>();
	RFTYPE_META().ComplexBaseClass<RightParent>();
}

RFTYPE_CREATE_META( RF::rftype::details::VirtualTop )
{
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualLeft )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualTop>();
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualRight )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualTop>();
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualFarLeft )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualLeft>();
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualFarRight )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualRight>();
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualDiamondCenter )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualLeft>();
	RFTYPE_META().ComplexBaseClass<VirtualRight>();
}
RFTYPE_CREATE_META( RF::rftype::details::VirtualExtremeDiamondCenter )
{
	using namespace ::RF::rftype::details;
	RFTYPE_META().ComplexBaseClass<VirtualFarLeft>();
	RFTYPE_META().ComplexBaseClass<VirtualFarRight>();
}

//////////
namespace RF::rftype { // Re-open namespace
////////
//////
////
//

///////////////////////////////////////////////////////////////////////////////

TEST( VirtualCast, Null )
{
	{
		// Keep const
		using Source = reflect::VirtualClass const;
		using Target = details::Parent const;
		Source* const source = nullptr;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Add const
		using Source = reflect::VirtualClass;
		using Target = details::Parent const;
		Source* const source = nullptr;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Non-const
		using Source = reflect::VirtualClass;
		using Target = details::Parent;
		Source* const source = nullptr;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
}



TEST( VirtualCast, Same )
{
	details::Parent root;
	{
		// Keep const
		using Source = reflect::VirtualClass const;
		using Target = details::Parent const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
	{
		// Add const
		using Source = reflect::VirtualClass;
		using Target = details::Parent const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
	{
		// Non-const
		using Source = reflect::VirtualClass;
		using Target = details::Parent;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
}



TEST( VirtualCast, Parent )
{
	details::Child root;
	{
		// Keep const
		using Source = reflect::VirtualClass const;
		using Target = details::Parent const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
	{
		// Add const
		using Source = reflect::VirtualClass;
		using Target = details::Parent const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
	{
		// Non-const
		using Source = reflect::VirtualClass;
		using Target = details::Parent;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
	}
}



TEST( VirtualCast, Child )
{
	details::Parent root;
	{
		// Keep const
		using Source = reflect::VirtualClass const;
		using Target = details::Child const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Add const
		using Source = reflect::VirtualClass;
		using Target = details::Child const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Non-const
		using Source = reflect::VirtualClass;
		using Target = details::Child;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
}



TEST( VirtualCast, Sibling )
{
	details::Child root;
	{
		// Keep const
		using Source = reflect::VirtualClass const;
		using Target = details::Sibling const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Add const
		using Source = reflect::VirtualClass;
		using Target = details::Sibling const;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
	{
		// Non-const
		using Source = reflect::VirtualClass;
		using Target = details::Sibling;
		Source* const source = &root;
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, nullptr );
	}
}



TEST( VirtualCast, MultipleParents )
{
	details::MultiChild root;
	root.leftPad = 1;
	root.rightPad = 2;
	root.childPad = 3;

	// NOTE: Need to disambiguate how to find VirtualClass from multiple
	//  non-virtual diamonding parents
	using Initial = details::MultiChild const;
	using Source = reflect::VirtualClass const;
	Initial* const initial = &root;

	// Multiple inheritance affects the equality checks of otherwise identical
	//  addresses through casting
	// NOTE: Checks on raw pointers are implementation-specific here, and may
	//  not be valid tests on other compilers and platforms
	using Raw = void const;

	{
		// Left, routed through left
		using Target = details::LeftParent const;
		using Routing = details::LeftParent const;
		Source* const source = static_cast<Routing*>( initial );
		ASSERT_EQ( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
		Target* const naturalFromInitial = static_cast<Target*>( initial );
		Target* const naturalFromSource = static_cast<Target*>( source );
		ASSERT_EQ( naturalFromInitial, naturalFromSource );
		ASSERT_EQ( naturalFromInitial->leftPad, root.leftPad );
		ASSERT_EQ( naturalFromSource->leftPad, root.leftPad );
		ASSERT_EQ( target, naturalFromInitial );
		ASSERT_EQ( target, naturalFromSource );
		ASSERT_EQ( target->leftPad, root.leftPad );
	}
	{
		// Left, routed through right
		using Target = details::LeftParent const;
		using Routing = details::RightParent const;
		Source* const source = static_cast<Routing*>( initial );
		// Routing through right causes offset
		ASSERT_NE( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_EQ( target, source );
		Target* const naturalFromInitial = static_cast<Target*>( initial );
		Target* const naturalFromSource = static_cast<Target*>( source );
		// Non-virtual routing causes corruption
		ASSERT_NE( naturalFromInitial, naturalFromSource );
		ASSERT_EQ( naturalFromInitial->leftPad, root.leftPad );
		// Corruption thinks right parent is left
		ASSERT_EQ( naturalFromSource->leftPad, root.rightPad );
		ASSERT_NE( target, naturalFromInitial );
		ASSERT_EQ( target, naturalFromSource );
		// Virtual_cast replicates corruption
		ASSERT_EQ( target->leftPad, root.rightPad );
	}
	{
		// Right, routed through right
		using Target = details::RightParent const;
		using Routing = details::RightParent const;
		Source* const source = static_cast<Routing*>( initial );
		// Routing through right causes offset
		ASSERT_NE( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_NE( target, nullptr );
		ASSERT_NE( target, source );
		Target* const naturalFromInitial = static_cast<Target*>( initial );
		Target* const naturalFromSource = static_cast<Target*>( source );
		ASSERT_EQ( naturalFromInitial, naturalFromSource );
		ASSERT_EQ( naturalFromInitial->rightPad, root.rightPad );
		ASSERT_EQ( naturalFromSource->rightPad, root.rightPad );
		// FAILURE: virtual_cast doesn't currently have enough information to
		//  identify which route was taken through the diamond, and tries to
		//  assume left, which results in improper offsets since they are done
		//  relative to the wrong route
		ASSERT_NE( target, naturalFromInitial );
		ASSERT_NE( target, naturalFromSource );
	}
	{
		// Right, routed through left
		using Target = details::RightParent const;
		using Routing = details::LeftParent const;
		Source* const source = static_cast<Routing*>( initial );
		ASSERT_EQ( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
		Target* const target = virtual_cast<Target*>( source );
		ASSERT_NE( target, nullptr );
		ASSERT_NE( target, source );
		Target* const naturalFromInitial = static_cast<Target*>( initial );
		Target* const naturalFromSource = static_cast<Target*>( source );
		// Non-virtual routing causes corruption
		ASSERT_NE( naturalFromInitial, naturalFromSource );
		ASSERT_EQ( naturalFromInitial->rightPad, root.rightPad );
		// Corruption thinks left parent is right
		ASSERT_EQ( naturalFromSource->rightPad, root.leftPad );
		ASSERT_EQ( target, naturalFromInitial );
		ASSERT_NE( target, naturalFromSource );
		// Virtual_cast avoids corruption because it assumes left routing
		ASSERT_EQ( target->rightPad, root.rightPad );
	}
}



// TODO: Non-empty virtual inheritance causes a lot of problems that are
//  unclear how to solve in a practical way, mostly due to the inability to
//  down-cast. A theoretical solution would be to embed the down-cast logic
//  into the virtual table as an optional extension when declaring the class,
//  but that's a lot of obtuse complexity to support a bad programming pattern,
//  and feels more like an academic proof-of-concept at this point...
TEST( VirtualCast, DISABLED_VirtualDiamond )
{
	details::VirtualTop const top = {};
	details::VirtualLeft const left = {};
	details::VirtualRight const right = {};
	details::VirtualFarLeft const farLeft = {};
	details::VirtualFarRight const farRight = {};
	details::VirtualDiamondCenter const diamondCenter = {};
	details::VirtualExtremeDiamondCenter const extremeDiamondCenter = {};

	using Source = reflect::VirtualClass const;

	// Virtual inheritance type knowledge affects the equality checks of
	//  otherwise identical addresses
	// NOTE: Checks on raw pointers are implementation-specific here, and may
	//  not be valid tests on other compilers and platforms
	using Raw = void const;

	// To top
	{
		using Target = details::VirtualTop const;
		{
			using Initial = details::VirtualTop const;
			Initial* const initial = &top;
			Source* const source = initial;
			ASSERT_EQ( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
			Target* const natural = static_cast<Target*>( initial );
			Target* const target = virtual_cast<Target*>( source );
			ASSERT_NE( target, nullptr );
			ASSERT_EQ( target, natural );
			ASSERT_EQ( target, source );
			ASSERT_EQ( reinterpret_cast<Raw*>( target ), reinterpret_cast<Raw*>( source ) );
		}
		{
			using Initial = details::VirtualLeft const;
			Initial* const initial = &left;
			Source* const source = initial;
			ASSERT_EQ( reinterpret_cast<Raw*>( source ), reinterpret_cast<Raw*>( initial ) );
			Target* const natural = static_cast<Target*>( initial );
			Target* const target = virtual_cast<Target*>( source );
			ASSERT_NE( target, nullptr );
			ASSERT_EQ( target, natural );
			ASSERT_EQ( target, source );
			ASSERT_EQ( reinterpret_cast<Raw*>( target ), reinterpret_cast<Raw*>( source ) );
		}
		// ...
	}
}



TEST( VirtualCast, ReflectNull )
{
	using Root = reflect::VirtualClassWithoutDestructor const;
	reflect::ClassInfo const& classInfo = *details::Parent{}.GetVirtualClassInfo();

	// Raw
	{
		void const* const null = nullptr;
		Root* const root = virtual_reflect_cast( classInfo, null );
		ASSERT_EQ( root, nullptr );
	}

	// Ptr
	{
		UniquePtr<void> null = nullptr;
		UniquePtr<Root> const root = virtual_reflect_ptr_cast( classInfo, rftl::move( null ) );
		ASSERT_EQ( root, nullptr );
	}
}



TEST( VirtualCast, ReflectBasic )
{
	using Root = reflect::VirtualClassWithoutDestructor const;

	{
		// Single hop up to virtual
		using Source = details::Parent const;

		// Raw
		{
			Source source = Source{};
			reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

			Root* const root = virtual_reflect_cast( classInfo, &source );
			ASSERT_NE( root, nullptr );
			Root* const compilerImpl = &source;
			ASSERT_EQ( root, compilerImpl );
			void const* voided = root;
			ASSERT_EQ( voided, &source );
		}

		// Ptr
		{
			UniquePtr<Source> source = DefaultCreator<Source>::Create();
			reflect::ClassInfo const& classInfo = *source->GetVirtualClassInfo();
			Source const* sourceAddr = source;

			UniquePtr<Root> const root = virtual_reflect_ptr_cast( classInfo, UniquePtr<void const>( rftl::move( source ) ) );
			ASSERT_NE( root, nullptr );
			Root* const compilerImpl = sourceAddr;
			ASSERT_EQ( root, compilerImpl );
			void const* voided = root;
			ASSERT_EQ( voided, sourceAddr );
		}
	}
	{
		// Double hop up to virtual
		using Source = details::Child const;

		// Raw
		{
			Source source = Source{};
			reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

			Root* const root = virtual_reflect_cast( classInfo, &source );
			ASSERT_NE( root, nullptr );
			Root* const compilerImpl = &source;
			ASSERT_EQ( root, compilerImpl );
			void const* voided = root;
			ASSERT_EQ( voided, &source );
		}

		// Ptr
		{
			UniquePtr<Source> source = DefaultCreator<Source>::Create();
			reflect::ClassInfo const& classInfo = *source->GetVirtualClassInfo();
			Source const* sourceAddr = source;

			UniquePtr<Root> const root = virtual_reflect_ptr_cast( classInfo, UniquePtr<void const>( rftl::move( source ) ) );
			ASSERT_NE( root, nullptr );
			Root* const compilerImpl = sourceAddr;
			ASSERT_EQ( root, compilerImpl );
			void const* voided = root;
			ASSERT_EQ( voided, sourceAddr );
		}
	}
}



TEST( VirtualCast, ReflectAmbiguous )
{
	using Root = reflect::VirtualClassWithoutDestructor const;

	{
		// Ambiguous garbage, can't determine path to root
		using Source = details::MultiChild const;

		// Raw
		{
			Source source = Source{};
			reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

			Root* const root = virtual_reflect_cast( classInfo, &source );
			ASSERT_EQ( root, nullptr );
			//Root* const compilerImpl = &source; // Compile error
		}

		// Ptr
		{
			UniquePtr<Source> source = DefaultCreator<Source>::Create();
			reflect::ClassInfo const& classInfo = *source->GetVirtualClassInfo();

			UniquePtr<Root> const root = virtual_reflect_ptr_cast( classInfo, UniquePtr<void const>( rftl::move( source ) ) );
			ASSERT_EQ( root, nullptr );
			// As with raw, this just doesn't work
		}
	}
}



TEST( VirtualCast, ReflectDiamond )
{
	using Root = reflect::VirtualClassWithoutDestructor const;

	{
		// Top
		using Source = details::VirtualTop const;
		Source source = Source{};
		reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

		Root* const root = virtual_reflect_cast( classInfo, &source );
		ASSERT_NE( root, nullptr );
		Root* const compilerImpl = &source;
		ASSERT_EQ( root, compilerImpl );
		void const* voided = root;
		ASSERT_NE( voided, &source ); // Implementation-dependent
	}
	{
		// Left
		using Source = details::VirtualLeft const;
		Source source = Source{};
		reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

		Root* const root = virtual_reflect_cast( classInfo, &source );
		ASSERT_NE( root, nullptr );
		Root* const compilerImpl = &source;
		ASSERT_EQ( root, compilerImpl );
		void const* voided = root;
		ASSERT_NE( voided, &source ); // Implementation-dependent
	}
	{
		// Right
		using Source = details::VirtualRight const;
		Source source = Source{};
		reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

		Root* const root = virtual_reflect_cast( classInfo, &source );
		ASSERT_NE( root, nullptr );
		Root* const compilerImpl = &source;
		ASSERT_EQ( root, compilerImpl );
		void const* voided = root;
		ASSERT_NE( voided, &source ); // Implementation-dependent
	}
	{
		// Center
		using Source = details::VirtualDiamondCenter const;
		Source source = Source{};
		reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

		Root* const root = virtual_reflect_cast( classInfo, &source );
		ASSERT_NE( root, nullptr );
		Root* const compilerImpl = &source;
		ASSERT_EQ( root, compilerImpl );
		void const* voided = root;
		ASSERT_NE( voided, &source ); // Implementation-dependent
	}
	{
		// Extreme center
		using Source = details::VirtualDiamondCenter const;
		Source source = Source{};
		reflect::ClassInfo const& classInfo = *source.GetVirtualClassInfo();

		Root* const root = virtual_reflect_cast( classInfo, &source );
		ASSERT_NE( root, nullptr );
		Root* const compilerImpl = &source;
		ASSERT_EQ( root, compilerImpl );
		void const* voided = root;
		ASSERT_NE( voided, &source ); // Implementation-dependent
	}
}

///////////////////////////////////////////////////////////////////////////////
}
