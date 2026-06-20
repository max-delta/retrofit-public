#pragma once
#include "core/macros.h"


// Forwards
namespace RF {
template<typename T, size_t LenT>
struct ConcatLiterals;
}

namespace RF {
///////////////////////////////////////////////////////////////////////////////
namespace literal::details {
// Workaround for finicky language lawyer issues in the C++ standard
struct WeakTag
{
};
}
///////////////////////////////////////////////////////////////////////////////

// Consteval reference to null-terminated string literal
template<typename T, size_t LenT>
struct LiteralRef
{
	RF_NO_COPY( LiteralRef );

public:
	using CharType = T;
	static constexpr size_t kLen = LenT;
	using ArrayType = CharType const[kLen];

private:
	template<typename U, size_t LenU>
	friend struct ConcatLiterals;
	template<typename U, size_t LLenU, size_t RLenU>
	friend constexpr ConcatLiterals<U, LLenU + RLenU - 1> operator+( LiteralRef<U, LLenU> const& lhs, U const ( &rhs )[RLenU] );
	template<typename U, size_t LLenU, size_t RLenU>
	friend constexpr ConcatLiterals<U, LLenU + RLenU - 1> operator+( ConcatLiterals<U, LLenU> const& lhs, U const ( &rhs )[RLenU] );


public:
	consteval LiteralRef( ArrayType& value )
		: LiteralRef( value, literal::details::WeakTag{} )
	{
		//
	}

	// Decay to array
	constexpr operator ArrayType&() const
	{
		return kRef;
	}


private:
	// NOTE: As constexpr as opposed to consteval, this has weak guarantees,
	//  but avoids finicky language lawyer issues in the C++ standard that
	//  apply when trying to use it for constexpr logic
	constexpr LiteralRef( ArrayType& value, literal::details::WeakTag )
		: kRef( value )
	{
		//
	}


public:
	ArrayType& kRef;
};

///////////////////////////////////////////////////////////////////////////////

// Empty literal
template<typename T>
consteval LiteralRef<T, 1> Literal()
{
	return LiteralRef<T, 1>{ "" };
}



// Literal from null-terminated array
template<typename T, size_t LenT>
consteval LiteralRef<T, LenT> Literal( T const ( &value )[LenT] )
{
	return LiteralRef<T, LenT>{ value };
}

///////////////////////////////////////////////////////////////////////////////

// Constexpr concatenation of null-terminated string literals
template<typename T, size_t LenT>
struct ConcatLiterals
{
	RF_NO_COPY( ConcatLiterals );

public:
	using CharType = T;
	static constexpr size_t kLen = LenT;
	using ArrayType = CharType const[kLen];
	using LiteralType = LiteralRef<CharType, kLen>;


public:
	template<size_t LenU>
	constexpr ConcatLiterals( LiteralRef<CharType, LenU> const& lhs, LiteralRef<CharType, kLen - LenU + 1> const& rhs )
		: kResultBuf{} // MSVC requires this to be initialized
		, kResult( kResultBuf, literal::details::WeakTag{} )
	{
		constexpr size_t kFirstBytes = LenU;
		constexpr size_t kSecondBytes = kLen - LenU + 1;

		constexpr size_t kStartOfFirstWrite = 0;
		for( size_t i = 0; i < kFirstBytes; i++ )
		{
			kResultBuf[kStartOfFirstWrite + i] = lhs[i];
		}

		constexpr size_t kStartOfSecondWrite = kFirstBytes - 1;
		for( size_t i = 0; i < kSecondBytes; i++ )
		{
			kResultBuf[kStartOfSecondWrite + i] = rhs[i];
		}

		kResultBuf[kLen - 1] = '\0';
	}

	// Decay to array
	constexpr operator ArrayType&() const
	{
		// Constexpr logic fails to chain casts, so cast is done here
		return kResult;
	}


public:
	CharType kResultBuf[kLen];
	LiteralType kResult;
};

///////////////////////////////////////////////////////////////////////////////

// Literal() + Literal()
template<typename T, size_t LLenT, size_t RLenT>
constexpr ConcatLiterals<T, LLenT + RLenT - 1> operator+( LiteralRef<T, LLenT> const& lhs, LiteralRef<T, RLenT> const& rhs )
{
	return ConcatLiterals<T, LLenT + RLenT - 1>{ lhs, rhs };
}

// Literal() + ""
template<typename T, size_t LLenT, size_t RLenT>
constexpr ConcatLiterals<T, LLenT + RLenT - 1> operator+( LiteralRef<T, LLenT> const& lhs, T const ( &rhs )[RLenT] )
{
	return ConcatLiterals<T, LLenT + RLenT - 1>{ lhs, { rhs, literal::details::WeakTag{} } };
}

// Literal() + "" + ""
template<typename T, size_t LLenT, size_t RLenT>
constexpr ConcatLiterals<T, LLenT + RLenT - 1> operator+( ConcatLiterals<T, LLenT> const& lhs, T const ( &rhs )[RLenT] )
{
	return ConcatLiterals<T, LLenT + RLenT - 1>{ lhs.kResult, { rhs, literal::details::WeakTag{} } };
}

///////////////////////////////////////////////////////////////////////////////
}
