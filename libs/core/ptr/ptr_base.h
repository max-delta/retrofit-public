#pragma once
#include "core/ptr/ptr_fwd.h"
#include "core/ptr/ptr_ref.h"
#include "core/ptr/creation_payload.h"
#include "core/ptr/ptr_traits.h"

#include "rftl/utility"


#define RF_PTR_ASSERT_CASTABLE(BASETYPE, DERIVEDTYPE) \
	static_assert( \
		rftl::is_base_of<BASETYPE, DERIVEDTYPE>::value, \
		"Failed to cast PTR<"#DERIVEDTYPE"> to PTR<"#BASETYPE">" ); \
	static_assert( \
		rftl::has_virtual_destructor<BASETYPE>::value || \
		rftl::is_base_of<::RF::PtrTrait::NoVirtualDestructor, BASETYPE>::value, \
		"Potentially unsafe deletions due to non-virtual destructor on base class. Please use 'virtual ~Base() = default;' at a minimum." );


namespace RF {
///////////////////////////////////////////////////////////////////////////////

template<typename T>
class PtrBase
{
	RF_NO_COPY( PtrBase );

	//
	// Types
public:
	typedef PtrRef<T> PtrRef;


	//
	// Public methods
public:
	PtrBase( CreationPayload<T> && payload )
		: m_Target(payload.m_Target)
		, m_Ref(payload.m_Ref)
	{
		payload.Clean();
	}

	template<typename DERIVED>
	PtrBase( CreationPayload<DERIVED> && payload )
		: m_Target(static_cast<T*>( payload.m_Target ))
		, m_Ref(reinterpret_cast<PtrRef*>( payload.m_Ref ))
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		payload.Clean();
	}


	//
	// Protected methods
protected:
	explicit PtrBase( T * target, PtrRef * ref )
		: m_Target(target)
		, m_Ref(ref)
	{
		//
	}

	template<typename DERIVED, typename PTRREFDERIVED>
	explicit PtrBase( DERIVED * target, PTRREFDERIVED * ref )
		: m_Target(static_cast<T*>(target))
		, m_Ref(reinterpret_cast<PtrRef*>(ref))
	{
		RF_PTR_ASSERT_CASTABLE( T, DERIVED );
		static_assert( rftl::is_same<PTRREFDERIVED, RF::PtrRef<DERIVED> >::value, "Expected to receive DERIVED* and PtrRef<DERIVED>*" );
	}

	explicit PtrBase( PtrBase && rhs )
		: PtrBase( nullptr, nullptr )
	{
		Swap( rftl::move( rhs ) );
	}

	void IncreaseStrongCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->IncreaseStrongCount();
		}
	}

	void DecreaseStrongCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->DecreaseStrongCount(m_Target);
		}
	}

	void IncreaseWeakCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->IncreaseWeakCount();
		}
	}

	void DecreaseWeakCount()
	{
		if( m_Ref != nullptr )
		{
			m_Ref->DecreaseWeakCount();
		}
	}

	void Swap( PtrBase && rhs )
	{
		rftl::swap( m_Ref, rhs.m_Ref );
		rftl::swap( m_Target, rhs.m_Target );
	}

	CreationPayload<T> CreateTransferPayload() const
	{
		return CreationPayload<T>( m_Target, m_Ref );
	}

	CreationPayload<T> CreateTransferPayloadAndWipeSelf()
	{
		T * const target = m_Target;
		PtrRef * const ref = m_Ref;
		m_Target = nullptr;
		m_Ref = nullptr;
		return CreationPayload<T>( target, ref );
	}

	void SanitizeTarget()
	{
		if( m_Target == nullptr )
		{
			return;
		}
		if( m_Ref->GetStrongCount() == 0 )
		{
			m_Target = nullptr;
		}
	}

	T * GetTarget() const
	{
		return m_Target;
	}

	T * GetTargetAsWeak() const
	{
		if( m_Target == nullptr )
		{
			return nullptr;
		}
		RF_ASSERT(m_Ref != nullptr);
		if( m_Ref->GetStrongCount() == 0 )
		{
			return nullptr;
		}
		return m_Target;
	}

	PtrRef * GetRef() const
	{
		return m_Ref;
	}


	//
	// Private data
private:
	T * m_Target;
	PtrRef * m_Ref;
};

///////////////////////////////////////////////////////////////////////////////
}
