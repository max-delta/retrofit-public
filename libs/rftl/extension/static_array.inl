#pragma once
#include "static_array.h"

#include "core/macros.h"

namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
inline Element& static_array<Element, ElementCapacity>::Storage::Value()
{
	return *( reinterpret_cast<Element*>( m_Data ) );
}



template<typename Element, size_t ElementCapacity>
inline Element const & static_array<Element, ElementCapacity>::Storage::Value() const
{
	return *( reinterpret_cast<Element const*>( m_Data ) );
}



template<typename Element, size_t ElementCapacity>
inline Element * static_array<Element, ElementCapacity>::Storage::WriteableTarget()
{
	return reinterpret_cast<Element*>( m_Data );
}

///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array()
	: m_CurrentSize( 0 )
{
	//
}



template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array( size_type count )
	: static_array()
{
	grow( count );
}



template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array( size_type count, value_type const & value )
	: static_array()
{
	grow( count, value );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline static_array<Element, ElementCapacity>::static_array( InputIterator first, InputIterator term )
	: static_array()
{
	append( first, term );
}



template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array( static_array const & other )
	: static_array( other.begin(), other.end() )
{
	//
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_array<Element, ElementCapacity>::static_array( static_array<value_type, OtherCapacity> const & other )
	: static_array( other.begin(), other.end() )
{
	//
}




template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array( static_array && other )
	: static_array()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename Element, size_t ElementCapacity>
template<size_t OtherCapacity>
inline static_array<Element, ElementCapacity>::static_array( static_array<value_type, OtherCapacity> && other )
	: static_array()
{
	extract( other.begin(), other.end() );
	other.clear();
}



template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::static_array( rftl::initializer_list<value_type> init )
	: static_array()
{
	append( init );
}



template<typename Element, size_t ElementCapacity>
inline static_array<Element, ElementCapacity>::~static_array()
{
	clear();
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::assign( size_type count, value_type const & value )
{
	clear();
	grow( count, value );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_array<Element, ElementCapacity>::assign( InputIterator first, InputIterator term )
{
	clear();
	append( first, term );
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::assign( rftl::initializer_list<value_type> init )
{
	clear();
	append( init );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::at( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reference static_array<Element, ElementCapacity>::at( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return ( *this )[pos];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::operator[]( size_type pos )
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos].Value();
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reference static_array<Element, ElementCapacity>::operator[]( size_type pos ) const
{
	RF_ASSERT_MSG( pos < size(), "Invalid pos accesses past end" );
	return m_Storage[pos].Value();
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::front()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reference static_array<Element, ElementCapacity>::front() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[0];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reference static_array<Element, ElementCapacity>::back() const
{
	RF_ASSERT_MSG( empty() == false, "Attempting access on empty container" );
	return ( *this )[size() - 1];
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::value_type * static_array<Element, ElementCapacity>::data()
{
	return &(m_Storage[0].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::value_type const * static_array<Element, ElementCapacity>::data() const
{
	return &(m_Storage[0].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::iterator static_array<Element, ElementCapacity>::begin()
{
	return &(m_Storage[0].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_iterator static_array<Element, ElementCapacity>::begin() const
{
	return &(m_Storage[0].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_iterator static_array<Element, ElementCapacity>::cbegin() const
{
	return begin();
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::iterator static_array<Element, ElementCapacity>::end()
{
	return &(m_Storage[size()].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_iterator static_array<Element, ElementCapacity>::end() const
{
	return &(m_Storage[size()].Value());
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_iterator static_array<Element, ElementCapacity>::cend() const
{
	return end();
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reverse_iterator static_array<Element, ElementCapacity>::rbegin()
{
	return reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reverse_iterator static_array<Element, ElementCapacity>::rbegin() const
{
	return const_reverse_iterator( end() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reverse_iterator static_array<Element, ElementCapacity>::crbegin() const
{
	return const_reverse_iterator( cend() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reverse_iterator static_array<Element, ElementCapacity>::rend()
{
	return reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reverse_iterator static_array<Element, ElementCapacity>::rend() const
{
	return const_reverse_iterator( begin() );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::const_reverse_iterator static_array<Element, ElementCapacity>::crend() const
{
	return const_reverse_iterator( cbegin() );
}



template<typename Element, size_t ElementCapacity>
inline bool static_array<Element, ElementCapacity>::empty() const
{
	return size() == 0;
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::size_type static_array<Element, ElementCapacity>::size() const
{
	RF_ASSERT( m_CurrentSize <= capacity() );
	return m_CurrentSize;
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::size_type static_array<Element, ElementCapacity>::max_size() const
{
	return fixed_capacity;
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::reserve( size_type newCap ) const
{
	RF_ASSERT_MSG( newCap <= max_size(), "Cannot grow beyond max size" );
	return;
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::size_type static_array<Element, ElementCapacity>::capacity() const
{
	return max_size();
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::shrink_to_fit()
{
	// No...
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::clear()
{
	while( empty() == false )
	{
		pop_back();
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::push_back( value_type const & value )
{
	emplace_back( value );
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::push_back( value_type && value )
{
	emplace_back( std::move( value ) );
}



template<typename Element, size_t ElementCapacity>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::emplace_back(value_type && value)
{
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	new ( m_Storage[m_CurrentSize].WriteableTarget() ) Element( std::move(value) );
	m_CurrentSize++;
	return back();
}



template<typename Element, size_t ElementCapacity>
template<class ...Args>
inline typename static_array<Element, ElementCapacity>::reference static_array<Element, ElementCapacity>::emplace_back( Args && ...args )
{
	RF_ASSERT_MSG( size() < capacity(), "Attempting grow on full container" );
	new ( m_Storage[m_CurrentSize].WriteableTarget() ) Element( args... );
	m_CurrentSize++;
	return back();
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::pop_back()
{
	RF_ASSERT_MSG( empty() == false, "Attempting pop_back on empty container" );
	RF_ASSERT( m_CurrentSize > 0 );
	m_CurrentSize--;
	m_Storage[m_CurrentSize].Value().~Element();
}

///////////////////////////////////////////////////////////////////////////////

template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_array<Element, ElementCapacity>::append( InputIterator first, InputIterator term )
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		emplace_back( std::move( *iter ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::append( rftl::initializer_list<value_type> init )
{
	append( init.begin(), init.end() );
}



template<typename Element, size_t ElementCapacity>
template<class InputIterator>
inline void static_array<Element, ElementCapacity>::extract(InputIterator first, InputIterator term)
{
	RF_ASSERT( first <= term );
	InputIterator iter = first;
	while( iter < term )
	{
		emplace_back( std::move( *iter ) );
		iter++;
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::grow( size_type growthAmount )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		emplace_back();
	}
}



template<typename Element, size_t ElementCapacity>
inline void static_array<Element, ElementCapacity>::grow( size_type growthAmount, value_type const & value )
{
	RF_ASSERT( size() + growthAmount < max_size() );
	for( size_type i = 0; i < growthAmount; i++ )
	{
		push_back( value );
	}
}

///////////////////////////////////////////////////////////////////////////////
}
