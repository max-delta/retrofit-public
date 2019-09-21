#pragma once
#include "core/compiler.h"

#include "rftl/initializer_list"
#include "rftl/iterator"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
class static_array
{
	//
	// Types
public:
	typedef Element value_type;
	typedef void allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef value_type const& const_reference;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
	typedef rftl::reverse_iterator<iterator> reverse_iterator;
	typedef rftl::reverse_iterator<const_iterator> const_reverse_iterator;


	//
	// Constants
public:
	static constexpr size_type fixed_capacity = ElementCapacity;


	//
	// Structs
private:
	struct alignas( alignof( Element ) ) Storage
	{
		Element& Value();
		Element const& Value() const;
		Element* WriteableTarget();

	private:
		static_assert( sizeof( Element ) > 0, "Container of empty elements is meaningless" );
		uint8_t mData[sizeof( Element )];
	};
	static_assert( alignof( Storage ) == alignof( Element ), "Unxepected alignment change" );
	static_assert( sizeof( Storage ) == sizeof( Element ), "Unxepected size change" );


	//
	// Public methods
public:
	static_array();
	explicit static_array( size_type count );
	static_array( size_type count, value_type const& value );
	template<class InputIterator>
	static_array( InputIterator first, InputIterator term );
	static_array( static_array const& other );
	template<size_t OtherCapacity>
	static_array( static_array<value_type, OtherCapacity> const& other );
	static_array( static_array&& other );
	template<size_t OtherCapacity>
	static_array( static_array<value_type, OtherCapacity>&& other );
	static_array( rftl::initializer_list<value_type> init );
	template<typename Convertible>
	static_array( rftl::initializer_list<Convertible> init );
	~static_array();

	static_array& operator=( static_array const& other );
	static_array& operator=( static_array const&& other );
	template<size_t OtherCapacity>
	static_array& operator=( static_array<value_type, OtherCapacity> const& other );
	template<size_t OtherCapacity>
	static_array& operator=( static_array<value_type, OtherCapacity>&& other );
	static_array& operator=( rftl::initializer_list<value_type> init );

	void assign( size_type count, value_type const& value );
	template<class InputIterator>
	void assign( InputIterator first, InputIterator term );
	void assign( rftl::initializer_list<value_type> init );

	reference at( size_type pos );
	const_reference at( size_type pos ) const;

	reference operator[]( size_type pos );
	const_reference operator[]( size_type pos ) const;

	reference front();
	const_reference front() const;

	reference back();
	const_reference back() const;

	value_type* data();
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
	size_type max_size() const;
	void reserve( size_type newCap ) const;
	size_type capacity() const;
	void shrink_to_fit();

	void clear();

	void push_back( value_type const& value );
	void push_back( value_type&& value );
	reference emplace_back( value_type&& value ); // Disambiguate args
	template<class... Args>
	reference emplace_back( Args&&... args );
	void pop_back();

	iterator insert( const_iterator pos, const value_type& value );
	iterator insert( const_iterator pos, value_type&& value );
	iterator erase( const_iterator pos );

	void resize( size_type count );
	void resize( size_type count, value_type const& value );


	//
	// Private methods
private:
	template<class InputIterator>
	void append( InputIterator first, InputIterator term );
	void append( rftl::initializer_list<value_type> init );
	template<typename Convertible>
	void append( rftl::initializer_list<Convertible> init );
	template<class InputIterator>
	void extract( InputIterator first, InputIterator term );
	void grow( size_type growthAmount );
	void grow( size_type growthAmount, value_type const& value );

	//
	// Private data
private:
	size_type m_CurrentSize;
	RF_ACK_32BIT_PADDING;
	Storage m_Storage[ElementCapacity];
};


template<typename Element, size_t LHSCapacity, size_t RHSCapacity>
bool operator==( static_array<Element, LHSCapacity> const& lhs, static_array<Element, LHSCapacity> const& rhs );
template<typename Element, size_t LHSCapacity, size_t RHSCapacity>
bool operator!=( static_array<Element, LHSCapacity> const& lhs, static_array<Element, LHSCapacity> const& rhs );

///////////////////////////////////////////////////////////////////////////////
}

#include "static_array.inl"
