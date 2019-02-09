#pragma once
#include "rftl/cstdint"
#include "rftl/type_traits"

namespace RF { namespace math {
///////////////////////////////////////////////////////////////////////////////

class Color3f final
{
	//
	// Types
public:
	using ElementType = float;


	//
	// Public methods
public:
	Color3f();
	Color3f( ElementType x, ElementType y, ElementType z );

	void Clamp();

	bool operator==( Color3f const& rhs ) const;

	Color3f operator+( Color3f const& rhs ) const;
	Color3f operator-( Color3f const& rhs ) const;
	Color3f& operator+=( Color3f const& rhs );
	Color3f& operator-=( Color3f const& rhs );

	Color3f operator+( ElementType const& rhs ) const;
	Color3f operator-( ElementType const& rhs ) const;
	Color3f operator*( ElementType const& rhs ) const;
	Color3f& operator+=( ElementType const& rhs );
	Color3f& operator-=( ElementType const& rhs );
	Color3f& operator*=( ElementType const& rhs );


	//
	// Public data
public:
	ElementType r;
	ElementType g;
	ElementType b;
};

///////////////////////////////////////////////////////////////////////////////
}}
