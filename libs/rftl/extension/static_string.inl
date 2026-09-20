#pragma once
#include "static_string.h"

#include "core/rf_assert.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::static_basic_string()
{
	//
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( size_type count, value_type const& value )
	: static_basic_string()
{
	grow( count, value );
}



template<typename CharT, size_t CapacityT>
template<class InputIterator>
inline static_basic_string<CharT, CapacityT>::static_basic_string( InputIterator first, InputIterator term )
	: static_basic_string()
{
	append( first, term );
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( static_basic_string const& other )
	: static_basic_string( other.begin(), other.end() )
{
	//
}



template<typename CharT, size_t CapacityT>
template<size_t OtherCapacity>
inline static_basic_string<CharT, CapacityT>::static_basic_string( static_basic_string<value_type, OtherCapacity> const& other )
	: static_basic_string( other.begin(), other.end() )
{
	//
}




template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( static_basic_string&& other )
	: static_basic_string()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename CharT, size_t CapacityT>
template<size_t OtherCapacity>
inline static_basic_string<CharT, CapacityT>::static_basic_string( static_basic_string<value_type, OtherCapacity>&& other )
	: static_basic_string()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( rftl::initializer_list<value_type> init )
	: static_basic_string()
{
	append( init );
}



template<typename CharT, size_t CapacityT>
template<typename ViewT>
	requires static_string_details::StringViewLike<CharT, ViewT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( ViewT const& other )
	: static_basic_string()
{
	append( other );
}



template<typename CharT, size_t CapacityT>
template<typename Convertible>
inline static_basic_string<CharT, CapacityT>::static_basic_string( rftl::initializer_list<Convertible> init )
	: static_basic_string()
{
	append( init );
}



template<typename CharT, size_t CapacityT>
template<typename IntegralT>
	requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
inline static_basic_string<CharT, CapacityT>::static_basic_string( IntegralT const& mem )
{
	assign( mem );
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::~static_basic_string()
{
	clear();
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator=( static_basic_string const& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
	}
	return *this;
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator=( static_basic_string const&& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
		other.clear();
	}
	return *this;
}



template<typename CharT, size_t CapacityT>
template<size_t OtherCapacity>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator=( static_basic_string<value_type, OtherCapacity> const& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
	}
	return *this;
}



template<typename CharT, size_t CapacityT>
template<size_t OtherCapacity>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator=( static_basic_string<value_type, OtherCapacity>&& other )
{
	if( &other != this )
	{
		assign( other.begin(), other.end() );
		other.clear();
	}
	return *this;
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& rftl::static_basic_string<CharT, CapacityT>::operator=( rftl::initializer_list<value_type> init )
{
	assign( init );
	return *this;
}



template<typename CharT, size_t CapacityT>
template<typename StringViewLike>
inline static_basic_string<CharT, CapacityT>& rftl::static_basic_string<CharT, CapacityT>::operator=( StringViewLike const& other )
{
	assign( other );
	return *this;
}



template<typename CharT, size_t CapacityT>
template<typename IntegralT>
	requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator=( IntegralT const& mem )
{
	assign( mem );
	return *this;
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::assign( size_type count, value_type const& value )
{
	clear();
	grow( count, value );
}



template<typename CharT, size_t CapacityT>
template<class InputIterator>
inline void static_basic_string<CharT, CapacityT>::assign( InputIterator first, InputIterator term )
{
	clear();
	append( first, term );
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::assign( rftl::initializer_list<value_type> init )
{
	clear();
	append( init );
}



template<typename CharT, size_t CapacityT>
template<typename ViewT>
	requires static_string_details::StringViewLike<CharT, ViewT>
inline void static_basic_string<CharT, CapacityT>::assign( ViewT const& other )
{
	clear();
	append( other );
}



template<typename CharT, size_t CapacityT>
template<typename IntegralT>
	requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
inline void static_basic_string<CharT, CapacityT>::assign( IntegralT const& mem )
{
	static_assert( sizeof( mStorage ) == sizeof( mem ) );
	memcpy( data(), &mem, sizeof( mem ) );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reference static_basic_string<CharT, CapacityT>::at( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reference static_basic_string<CharT, CapacityT>::at( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reference static_basic_string<CharT, CapacityT>::operator[]( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return mStorage[pos];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reference static_basic_string<CharT, CapacityT>::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return mStorage[pos];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reference static_basic_string<CharT, CapacityT>::front()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reference static_basic_string<CharT, CapacityT>::front() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reference static_basic_string<CharT, CapacityT>::back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reference static_basic_string<CharT, CapacityT>::back() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::value_type* static_basic_string<CharT, CapacityT>::data()
{
	return &( mStorage[0] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::value_type const* static_basic_string<CharT, CapacityT>::data() const
{
	return &( mStorage[0] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::value_type const* static_basic_string<CharT, CapacityT>::c_str() const
{
	return data();
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>::operator basic_string_view<CharT>() const
{
	return basic_string_view<CharT>( data(), size() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::iterator static_basic_string<CharT, CapacityT>::begin()
{
	return &( mStorage[0] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_iterator static_basic_string<CharT, CapacityT>::begin() const
{
	return &( mStorage[0] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_iterator static_basic_string<CharT, CapacityT>::cbegin() const
{
	return begin();
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::iterator static_basic_string<CharT, CapacityT>::end()
{
	return &( mStorage[size()] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_iterator static_basic_string<CharT, CapacityT>::end() const
{
	return &( mStorage[size()] );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_iterator static_basic_string<CharT, CapacityT>::cend() const
{
	return end();
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reverse_iterator static_basic_string<CharT, CapacityT>::rbegin()
{
	return reverse_iterator( end() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reverse_iterator static_basic_string<CharT, CapacityT>::rbegin() const
{
	return const_reverse_iterator( end() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reverse_iterator static_basic_string<CharT, CapacityT>::crbegin() const
{
	return const_reverse_iterator( cend() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::reverse_iterator static_basic_string<CharT, CapacityT>::rend()
{
	return reverse_iterator( begin() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reverse_iterator static_basic_string<CharT, CapacityT>::rend() const
{
	return const_reverse_iterator( begin() );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::const_reverse_iterator static_basic_string<CharT, CapacityT>::crend() const
{
	return const_reverse_iterator( cbegin() );
}



template<typename CharT, size_t CapacityT>
inline bool static_basic_string<CharT, CapacityT>::empty() const
{
	return size() == 0;
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::size_type static_basic_string<CharT, CapacityT>::size() const
{
	for( size_t i = 0; i < capacity(); i++ )
	{
		if( mStorage[i] == CharT{} )
		{
			return i;
		}
	}
	return capacity();
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::size_type static_basic_string<CharT, CapacityT>::length() const
{
	return size();
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::size_type static_basic_string<CharT, CapacityT>::max_size() const
{
	return fixed_capacity;
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::reserve( size_type newCap ) const
{
	RF_ASSERT_MSG( newCap <= max_size(), "Cannot grow beyond max size" );
	return;
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::size_type static_basic_string<CharT, CapacityT>::capacity() const
{
	return max_size();
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::shrink_to_fit()
{
	// No...
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::clear()
{
	while( empty() == false )
	{
		pop_back();
	}
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::push_back( value_type const& value )
{
	value_type copy = value;
	push_back( rftl::move( copy ) );
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::push_back( value_type&& value )
{
	RF_ASSERT_MSG( value != CharT{}, "Null characters not supported" );
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	mStorage[size()] = rftl::move( value );
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::pop_back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting pop_back on empty container" );
	RF_ASSERT( size() > 0 );
	mStorage[size() - 1] = {};
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::iterator static_basic_string<CharT, CapacityT>::insert( const_iterator pos, const value_type& value )
{
	value_type copy = value;
	return insert( rftl::move( copy ) );
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::iterator static_basic_string<CharT, CapacityT>::insert( const_iterator pos, value_type&& value )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( size() < fixed_capacity );
	for( iterator iter = end(); iter > pos; iter-- )
	{
		iterator const dest = iter;
		iterator const source = iter - 1;
		*dest = rftl::move( *source );
		*source = CharT{};
	}
	iterator const mutablePos = const_cast<iterator>( pos );
	*mutablePos = rftl::move( value );
	return mutablePos;
}



template<typename CharT, size_t CapacityT>
inline typename static_basic_string<CharT, CapacityT>::iterator static_basic_string<CharT, CapacityT>::erase( const_iterator pos )
{
	RF_ASSERT( pos >= begin() );
	RF_ASSERT( pos <= end() );
	RF_ASSERT( size() > 0 );
	iterator const mutablePos = const_cast<iterator>( pos );
	for( iterator iter = mutablePos; iter <= end() - 1; iter++ )
	{
		iterator dest = iter;
		iterator source = iter + 1;
		*dest = rftl::move( *source );
		*source = CharT{};
	}
	return mutablePos;
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator+=( static_basic_string const& other )
{
	append( other.begin(), other.end() );
	return *this;
}



template<typename CharT, size_t CapacityT>
template<size_t OtherCapacity>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator+=( static_basic_string<value_type, OtherCapacity> const& other )
{
	append( other.begin(), other.end() );
	return *this;
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator+=( value_type const& value )
{
	push_back( value );
	return *this;
}



template<typename CharT, size_t CapacityT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator+=( rftl::initializer_list<value_type> init )
{
	append( init );
	return *this;
}



template<typename CharT, size_t CapacityT>
template<typename ViewT>
	requires static_string_details::StringViewLike<CharT, ViewT>
inline static_basic_string<CharT, CapacityT>& static_basic_string<CharT, CapacityT>::operator+=( ViewT const& other )
{
	append( other );
	return *this;
}



template<typename CharT, size_t CapacityT>
template<typename IntegralT>
	requires static_string_details::IntegerEquivalent<CharT[CapacityT], IntegralT>
inline IntegralT static_basic_string<CharT, CapacityT>::as_integer() const
{
	static_assert( sizeof( mStorage ) == sizeof( IntegralT ) );
	IntegralT retVal = {};
	memcpy( &retVal, data(), sizeof( retVal ) );
	return retVal;
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::resize( size_type count, value_type const& value )
{
	while( size() > count )
	{
		pop_back();
	}
	while( size() < count )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////

template<typename CharT, size_t CapacityT>
template<class InputIterator>
inline void static_basic_string<CharT, CapacityT>::append( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		push_back( rftl::move( *iter ) );
		iter++;
	}
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::append( rftl::initializer_list<value_type> init )
{
	append( init.begin(), init.end() );
}



template<typename CharT, size_t CapacityT>
template<typename ViewT>
	requires static_string_details::StringViewLike<CharT, ViewT>
inline void static_basic_string<CharT, CapacityT>::append( ViewT const& other )
{
	rftl::basic_string_view<CharT> const view( other );
	append( view.begin(), view.end() );
}



template<typename CharT, size_t CapacityT>
template<typename Convertible>
inline void static_basic_string<CharT, CapacityT>::append( rftl::initializer_list<Convertible> init )
{
	append( init.begin(), init.end() );
}



template<typename CharT, size_t CapacityT>
template<class InputIterator>
inline void static_basic_string<CharT, CapacityT>::extract( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		push_back( rftl::move( *iter ) );
		iter++;
	}
}



template<typename CharT, size_t CapacityT>
inline void static_basic_string<CharT, CapacityT>::grow( size_type growthAmount, value_type const& value )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
