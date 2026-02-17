#pragma once
#include "rftl/extension/static_vector.h"
#include "rftl/extension/versioned_iterator.h"
#include "rftl/optional"
#include "rftl/vector"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Continguous dynamic array that will attempt to use a fixed block of non-heap
//  memory until it can no longer fit, and will then 'stretch' to using heap
//  memory instead
template<typename Element, size_t ElementCapacity, size_t Alignment = alignof( Element )>
class stretch_vector
{
	//
	// Types and constants
public:
	typedef Element value_type;
	typedef void allocator_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef value_type& reference;
	typedef value_type const& const_reference;
	typedef value_type* pointer;
	typedef value_type const* const_pointer;
#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	typedef versioned_iterator<value_type> iterator;
	typedef versioned_iterator<value_type const> const_iterator;
#else
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
#endif
	typedef rftl::reverse_iterator<iterator> reverse_iterator;
	typedef rftl::reverse_iterator<const_iterator> const_reverse_iterator;

	static constexpr size_type fast_capacity = ElementCapacity;
	static constexpr size_type alignment = Alignment;
private:
	using underlying_fixed = static_vector<value_type, fast_capacity, alignment>;
	using underlying_stretch = vector<value_type>;


	//
	// Public methods
public:
	stretch_vector();
	explicit stretch_vector( size_type count );
	stretch_vector( size_type count, value_type const& value );
	template<class InputIterator>
	stretch_vector( InputIterator first, InputIterator term );
	stretch_vector( stretch_vector const& other );
	template<size_t OtherCapacity>
	stretch_vector( stretch_vector<value_type, OtherCapacity> const& other );
	stretch_vector( stretch_vector&& other );
	template<size_t OtherCapacity>
	stretch_vector( stretch_vector<value_type, OtherCapacity>&& other );
	stretch_vector( rftl::initializer_list<value_type> init );
	template<typename Convertible>
	stretch_vector( rftl::initializer_list<Convertible> init );
	~stretch_vector();

	stretch_vector& operator=( stretch_vector const& other );
	stretch_vector& operator=( stretch_vector&& other );
	template<size_t OtherCapacity>
	stretch_vector& operator=( stretch_vector<value_type, OtherCapacity> const& other );
	template<size_t OtherCapacity>
	stretch_vector& operator=( stretch_vector<value_type, OtherCapacity>&& other );
	stretch_vector& operator=( rftl::initializer_list<value_type> init );

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
	iterator erase( const_iterator first, const_iterator last );

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
	void stretch();

	iterator make_iter( value_type& value ) const;
	const_iterator make_const_iter( value_type const& value ) const;
	void invalidate();

	//
	// Private data
private:
	underlying_fixed mFixed = {};
	optional<underlying_stretch> mStretch = nullopt;
#if RF_IS_ALLOWED( RF_CONFIG_ASSERTS )
	container_version mVersion = {};
#endif
};

///////////////////////////////////////////////////////////////////////////////
}

#include "stretch_vector.inl"
