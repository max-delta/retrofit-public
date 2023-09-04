#pragma once
#include "rftl/extension/void_alias.h"
#include "rftl/extension/void_traits.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

class byte_view
{
	//
	// Types
public:
	typedef void value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type const* iterator;
	typedef value_type const* const_iterator;
	typedef rftl::reverse_iterator<void_alias const*> reverse_iterator;
	typedef rftl::reverse_iterator<void_alias const*> const_reverse_iterator;


	//
	// Public methods
public:
	byte_view() = default;
	byte_view( void* data, size_type size );
	byte_view( void const* data, size_type size );
	byte_view( nullptr_t data, size_type size );
	byte_view( const_iterator first, const_iterator term );
	template<class InputIterator>
	byte_view( InputIterator first, InputIterator term );
	template<class Type>
	explicit byte_view( Type const* data );
	byte_view( byte_view const& ) = default;
	byte_view( byte_view&& ) = default;
	~byte_view() = default;

	byte_view& operator=( byte_view const& ) = default;
	byte_view& operator=( byte_view&& ) = default;

	template<typename Type>
	Type const& at( size_type pos ) const;
	template<typename Type>
	Type const& operator[]( size_type pos ) const;
	template<typename Type>
	Type const& front() const;
	template<typename Type>
	Type const& back() const;

	value_type const* data() const;

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;

	iterator end();
	const_iterator end() const;
	const_iterator cend() const;

	reverse_iterator rbegin();
	const_reverse_iterator rbegin() const;
	const_reverse_iterator crbegin() const;

	reverse_iterator rend();
	const_reverse_iterator rend() const;
	const_reverse_iterator crend() const;

	bool empty() const;
	size_type size() const;
	size_type length() const;
	constexpr size_type max_size() const;

	void remove_prefix( size_type size );
	void remove_suffix( size_type size );
	byte_view substr( size_type pos );
	byte_view substr( size_type pos, size_type size );

	void* mem_copy_to( void* dest, size_t size ) const;


	//
	// Private data
private:
	const_pointer mData = nullptr;
	size_type mSize = 0;
};


bool operator==( byte_view const& lhs, byte_view const& rhs );
bool operator!=( byte_view const& lhs, byte_view const& rhs );

///////////////////////////////////////////////////////////////////////////////
}

#include "byte_view.inl"
