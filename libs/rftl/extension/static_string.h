#pragma once
#include "core/compiler.h"

#include "rftl/initializer_list"
#include "rftl/iterator"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// NOTE: Cannot store null characters
template<typename Element, size_t ElementCapacity>
class static_basic_string
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
	// Public methods
public:
	static_basic_string();
	static_basic_string( size_type count, value_type const& value );
	template<class InputIterator>
	static_basic_string( InputIterator first, InputIterator term );
	static_basic_string( static_basic_string const& other );
	template<size_t OtherCapacity>
	static_basic_string( static_basic_string<value_type, OtherCapacity> const& other );
	static_basic_string( static_basic_string&& other );
	template<size_t OtherCapacity>
	static_basic_string( static_basic_string<value_type, OtherCapacity>&& other );
	static_basic_string( rftl::initializer_list<value_type> init );
	template<typename Convertible>
	static_basic_string( rftl::initializer_list<Convertible> init );
	~static_basic_string();

	static_basic_string& operator=( static_basic_string const& other );
	static_basic_string& operator=( static_basic_string const&& other );
	template<size_t OtherCapacity>
	static_basic_string& operator=( static_basic_string<value_type, OtherCapacity> const& other );
	template<size_t OtherCapacity>
	static_basic_string& operator=( static_basic_string<value_type, OtherCapacity>&& other );
	static_basic_string& operator=( rftl::initializer_list<value_type> init );

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

	value_type const* c_str() const;

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
	size_type max_size() const;
	void reserve( size_type newCap ) const;
	size_type capacity() const;
	void shrink_to_fit();

	void clear();

	void push_back( value_type const& value );
	void push_back( value_type&& value );
	void pop_back();

	iterator insert( const_iterator pos, const value_type& value );
	iterator insert( const_iterator pos, value_type&& value );
	iterator erase( const_iterator pos );

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
	void grow( size_type growthAmount, value_type const& value );

	//
	// Private data
private:
	Element m_Storage[ElementCapacity] = {};
	Element const mNullTerminator = {};
};


template<typename Element, size_t LHSCapacity, size_t RHSCapacity>
bool operator==( static_basic_string<Element, LHSCapacity> const& lhs, static_basic_string<Element, LHSCapacity> const& rhs );
template<typename Element, size_t LHSCapacity, size_t RHSCapacity>
bool operator!=( static_basic_string<Element, LHSCapacity> const& lhs, static_basic_string<Element, LHSCapacity> const& rhs );

template<size_t ElementCapacity> using static_string = static_basic_string<char, ElementCapacity>;
template<size_t ElementCapacity> using static_wstring = static_basic_string<wchar_t, ElementCapacity>;
template<size_t ElementCapacity> using static_u16string = static_basic_string<char16_t, ElementCapacity>;
template<size_t ElementCapacity> using static_u32string = static_basic_string<char32_t, ElementCapacity>;

///////////////////////////////////////////////////////////////////////////////
}

#include "static_string.inl"
