#pragma once
#include "core/compiler.h"

#include "rftl/initializer_list"
#include "rftl/iterator"
#include "rftl/format"
#include "rftl/string_view"
#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////
namespace static_string_details {
template<typename CharT, typename ViewT>
concept StringViewLike = requires( ViewT const& t ) { basic_string_view<CharT>( t ); };

template<typename ArrayT, typename IntegralT>
concept IntegerEquivalent =
	is_integral<IntegralT>::value &&
	is_unsigned<IntegralT>::value &&
	sizeof( IntegralT ) == sizeof( ArrayT );
}
///////////////////////////////////////////////////////////////////////////////

// Statically-size storage for strings
// NOTE: Cannot store null characters
// NOTE: If sized to match an integral type, allows easy conversion for storage
// NOTE: When trying to match to an integer, note that the string is one
//  character larger, to enforce that a null character is present, which makes
//  the string poorly suited to storage needs since it slightly more wasteful
//  than storing the integer type
template<typename CharT, size_t CapacityT>
class static_basic_string
{
	//
	// Types
public:
	typedef CharT value_type;
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
	static constexpr size_type fixed_capacity = CapacityT;


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
	template<typename ViewT>
		requires static_string_details::StringViewLike<CharT, ViewT>
	explicit static_basic_string( ViewT const& other );
	template<typename Convertible>
	static_basic_string( rftl::initializer_list<Convertible> init );
	template<typename IntegralT>
		requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
	explicit static_basic_string( IntegralT const& mem );
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
	template<typename IntegralT>
		requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
	static_basic_string& operator=( IntegralT const& mem );

	void assign( size_type count, value_type const& value );
	template<class InputIterator>
	void assign( InputIterator first, InputIterator term );
	void assign( rftl::initializer_list<value_type> init );
	template<typename ViewT>
		requires static_string_details::StringViewLike<CharT, ViewT>
	void assign( ViewT const& other );
	template<typename IntegralT>
		requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
	void assign( IntegralT const& mem );

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
	operator basic_string_view<CharT>() const;

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
	template<typename ViewT>
		requires static_string_details::StringViewLike<CharT, ViewT>
	static_basic_string& operator+=( ViewT const& other );

	void resize( size_type count, value_type const& value );

	template<typename IntegralT>
		requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
	IntegralT as_integer() const;


	//
	// Private methods
private:
	template<class InputIterator>
	void append( InputIterator first, InputIterator term );
	void append( rftl::initializer_list<value_type> init );
	template<typename ViewT>
		requires static_string_details::StringViewLike<CharT, ViewT>
	void append( ViewT const& other );
	template<typename Convertible>
	void append( rftl::initializer_list<Convertible> init );
	template<class InputIterator>
	void extract( InputIterator first, InputIterator term );
	void grow( size_type growthAmount, value_type const& value );

	//
	// Private data
private:
	CharT mStorage[CapacityT] = {};
	CharT const mNullTerminator = {};
};


template<typename CharT, size_t LHSCapacity, size_t RHSCapacity>
bool operator==( static_basic_string<CharT, LHSCapacity> const& lhs, static_basic_string<CharT, LHSCapacity> const& rhs );
template<typename CharT, size_t LHSCapacity, size_t RHSCapacity>
bool operator!=( static_basic_string<CharT, LHSCapacity> const& lhs, static_basic_string<CharT, LHSCapacity> const& rhs );

template<size_t CapacityT> using static_string = static_basic_string<char, CapacityT>;
template<size_t CapacityT> using static_wstring = static_basic_string<wchar_t, CapacityT>;
template<size_t CapacityT> using static_u8string = static_basic_string<char8_t, CapacityT>;
template<size_t CapacityT> using static_u16string = static_basic_string<char16_t, CapacityT>;
template<size_t CapacityT> using static_u32string = static_basic_string<char32_t, CapacityT>;

///////////////////////////////////////////////////////////////////////////////
}

// Formats as a basic_string_view
template<typename CharT, size_t CapacityT, typename CtxCharT>
struct rftl::formatter<rftl::static_basic_string<CharT, CapacityT>, CtxCharT> : rftl::formatter<rftl::basic_string_view<CharT>, CtxCharT>
{
	using Input = rftl::static_basic_string<CharT, CapacityT>;
	using Shim = rftl::basic_string_view<CharT>;
	using Base = rftl::formatter<Shim, CtxCharT>;

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
