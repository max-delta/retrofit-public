#pragma once
#include "core/rf_assert.h"

#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

class container_version
{
	//
	// Public methods
public:
	size_t get() const
	{
		return mVer;
	}

	void invalidate()
	{
		mVer++;
	}

	bool operator==( container_version const& rhs ) const
	{
		return mVer == rhs.mVer;
	}


	//
	// Private data
private:
	size_t mVer = 0;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Element>
class versioned_iterator
{
	//
	// Friends
private:
	template<typename AnyT> friend class versioned_iterator;

	//
	// Types
public:
	using value_type = Element;


	//
	// Public methods
public:
	versioned_iterator() = default;

	versioned_iterator( Element* ptr, container_version const& container )
		: mPtr( ptr )
		, mContainerVersion( &container )
		, mIteratorVersion( container )
	{
		//
	}

	template<typename NonConstIterT,
		typename enable_if<
			conjunction<
				negation<
					is_const<
						typename NonConstIterT::value_type>>,
				is_const<value_type>>::value,
			int>::type = 0>
	versioned_iterator( NonConstIterT const& other )
		: mPtr( other.mPtr )
		, mContainerVersion( other.mContainerVersion )
		, mIteratorVersion( other.mIteratorVersion )
	{
		//
	}

	versioned_iterator operator+( ptrdiff_t offset ) const
	{
		versioned_iterator temp = *this;
		temp += offset;
		return temp;
	}

	versioned_iterator operator-( ptrdiff_t offset ) const
	{
		versioned_iterator temp = *this;
		temp -= offset;
		return temp;
	}

	ptrdiff_t operator-( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr - rhs.mPtr;
	}

	versioned_iterator& operator+=( ptrdiff_t offset )
	{
		mPtr += offset;
		return *this;
	}

	versioned_iterator& operator-=( ptrdiff_t offset )
	{
		mPtr -= offset;
		return *this;
	}

	versioned_iterator& operator++()
	{
		mPtr++;
		return *this;
	}

	versioned_iterator& operator--()
	{
		mPtr--;
		return *this;
	}

	versioned_iterator operator++( int )
	{
		versioned_iterator const old = *this;
		mPtr++;
		return old;
	}

	versioned_iterator operator--( int )
	{
		versioned_iterator const old = *this;
		mPtr--;
		return old;
	}

	Element& operator*() const
	{
		RF_ASSERT( mPtr != nullptr );
		RF_ASSERT( mContainerVersion != nullptr );
		RF_ASSERT( *mContainerVersion == mIteratorVersion );
		return *mPtr;
	}

	bool operator<( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr < rhs.mPtr;
	}

	bool operator<=( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr <= rhs.mPtr;
	}

	bool operator==( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr == rhs.mPtr;
	}

	bool operator>=( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr >= rhs.mPtr;
	}

	bool operator>( versioned_iterator const& rhs ) const
	{
		assert_compatibility( rhs );
		return mPtr > rhs.mPtr;
	}



	//
	// Private methods
private:
	void assert_compatibility( versioned_iterator const& other ) const
	{
		RF_ASSERT( mIteratorVersion == other.mIteratorVersion );
		RF_ASSERT( mContainerVersion == other.mContainerVersion );
	}


	//
	// Private data
private:
	Element* mPtr = nullptr;
	container_version const* mContainerVersion = nullptr;
	container_version mIteratorVersion = {};
};

///////////////////////////////////////////////////////////////////////////////
}
