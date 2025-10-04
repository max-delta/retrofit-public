#pragma once
#include "rftl/extension/void_alias.h"
#include "rftl/extension/void_traits.h"
#include "rftl/span"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Mutable span of bytes, which retains a 'void' concept and loose typing,
//  intended for use in buffer concepts
class byte_span
{
	//
	// Types
public:
	typedef void value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
	typedef rftl::reverse_iterator<void_alias*> reverse_iterator;
	typedef rftl::reverse_iterator<void_alias const*> const_reverse_iterator;


	//
	// Public methods
public:
	byte_span() = default;
	byte_span( void* data, size_type size );
	byte_span( nullptr_t data, size_type size );
	byte_span( iterator first, iterator term );
	template<class InputIterator>
	byte_span( InputIterator first, InputIterator term );
	template<class Type>
	explicit byte_span( Type* data );
	byte_span( byte_span const& ) = default;
	byte_span( byte_span&& ) = default;
	~byte_span() = default;

	template<typename T>
	span<T, dynamic_extent> to_typed_span() const;

	byte_span& operator=( byte_span const& ) = default;
	byte_span& operator=( byte_span&& ) = default;

	template<typename Type>
	Type& at( size_type pos ) const;
	template<typename Type>
	Type& operator[]( size_type pos ) const;
	template<typename Type>
	Type& front() const;
	template<typename Type>
	Type& back() const;

	value_type* data() const;

	iterator begin() const;
	const_iterator cbegin() const;

	iterator end() const;
	const_iterator cend() const;

	reverse_iterator rbegin() const;
	const_reverse_iterator crbegin() const;

	reverse_iterator rend() const;
	const_reverse_iterator crend() const;

	bool empty() const;
	size_type size() const;
	size_type length() const;
	constexpr size_type max_size() const;

	void remove_prefix( size_type size );
	void remove_suffix( size_type size );
	byte_span subspan( size_type pos ) const;
	byte_span subspan( size_type pos, size_type size ) const;

	template<typename Type>
	Type extract_front();
	template<typename Type>
	Type extract_back();

	void* mem_copy_to( void* dest, size_t size ) const;
	void* mem_copy_prefix_to( void* dest, size_t size ) const;

	template<typename Container>
	void* mem_copy_to( Container& container ) const;
	template<typename Container>
	void* mem_copy_prefix_to( Container& container ) const;


	//
	// Private data
private:
	pointer mData = nullptr;
	size_type mSize = 0;
};


bool operator==( byte_span const& lhs, byte_span const& rhs );
bool operator!=( byte_span const& lhs, byte_span const& rhs );

///////////////////////////////////////////////////////////////////////////////
}

#include "byte_span.inl"
