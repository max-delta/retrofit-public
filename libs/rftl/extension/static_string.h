#pragma once
#include "core/compiler.h"

#include "rftl/initializer_list"
#include "rftl/iterator"
#include "rftl/format"
#include "rftl/string_view"


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
	template<typename StringViewLike>
	explicit static_basic_string( StringViewLike const& other );
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
	template<typename StringViewLike>
	static_basic_string& operator=( StringViewLike const& other );

	void assign( size_type count, value_type const& value );
	template<class InputIterator>
	void assign( InputIterator first, InputIterator term );
	void assign( rftl::initializer_list<value_type> init );
	template<typename StringViewLike>
	void assign( StringViewLike const& other );

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
	operator basic_string_view<Element>() const;

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

	static_basic_string& operator+=( static_basic_string const& other );
	template<size_t OtherCapacity>
	static_basic_string& operator+=( static_basic_string<value_type, OtherCapacity> const& other );
	static_basic_string& operator+=( value_type const& value );
	static_basic_string& operator+=( rftl::initializer_list<value_type> init );
	template<typename StringViewLike>
	static_basic_string& operator+=( StringViewLike const& other );

	void resize( size_type count, value_type const& value );


	//
	// Private methods
private:
	template<class InputIterator>
	void append( InputIterator first, InputIterator term );
	void append( rftl::initializer_list<value_type> init );
	template<typename StringViewLike>
	void append( StringViewLike const& other );
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
template<size_t ElementCapacity> using static_u8string = static_basic_string<char8_t, ElementCapacity>;
template<size_t ElementCapacity> using static_u16string = static_basic_string<char16_t, ElementCapacity>;
template<size_t ElementCapacity> using static_u32string = static_basic_string<char32_t, ElementCapacity>;

///////////////////////////////////////////////////////////////////////////////
}

// Formats as a basic_string_view
template<typename CharT, size_t ElementCapacity>
struct rftl::formatter<rftl::static_basic_string<CharT, ElementCapacity>, char> : rftl::formatter<std::basic_string_view<CharT>, char>
{
	using Input = rftl::static_basic_string<CharT, ElementCapacity>;
	using Shim = rftl::basic_string_view<CharT>;
	using Base = rftl::formatter<Shim, char>;

	template<class ParseContext>
	constexpr typename ParseContext::iterator parse( ParseContext& ctx )
	{
		return Base::parse( ctx );
	}

	template<class FmtContext>
	typename FmtContext::iterator format( Input const& arg, FmtContext& ctx ) const
	{
		return Base::format( static_cast<Shim>( arg ), ctx );
	}
};

#include "static_string.inl"
