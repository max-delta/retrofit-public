#pragma once
#include "core/macros.h"


namespace RF {
///////////////////////////////////////////////////////////////////////////////

// Constexpr reference to null-terminated string literal
template<typename T, size_t LenT>
struct LiteralRef
{
	RF_NO_COPY( LiteralRef );

	using CharType = T;
	static constexpr size_t kLen = LenT;
	using ArrayType = CharType const[kLen];

	constexpr LiteralRef( ArrayType& value )
		: kRef( value )
	{
		//
	}

	constexpr CharType const& operator[]( size_t i ) const
	{
		return kRef[i];
	}

	constexpr operator ArrayType&() const
	{
		return kRef;
	}

	ArrayType& kRef;
};

///////////////////////////////////////////////////////////////////////////////

// Empty literal
template<typename T>
constexpr LiteralRef<T, 1> Literal()
{
	return LiteralRef<T, 1>{ "" };
}



// Literal from null-terminated array
template<typename T, size_t LenT>
constexpr LiteralRef<T, LenT> Literal( T const ( &value )[LenT] )
{
	return LiteralRef<T, LenT>{ value };
}

///////////////////////////////////////////////////////////////////////////////

// Constexpr concatenation of null-terminated string literals
template<typename T, size_t LenT>
struct ConcatLiterals
{
	RF_NO_COPY( ConcatLiterals );

	using CharType = T;
	static constexpr size_t kLen = LenT;
	using ArrayType = CharType const[kLen];

	char kResultBuf[kLen];
	LiteralRef<CharType, kLen> kResult;

	template<size_t LenU>
	constexpr ConcatLiterals( LiteralRef<CharType, LenU> const& lhs, LiteralRef<CharType, kLen - LenU + 1> const& rhs )
		: kResultBuf{} // MSVC requires this to be initialized
		, kResult( kResultBuf )
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

	constexpr operator ArrayType&() const
	{
		return kResult;
	}
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
	return ConcatLiterals<T, LLenT + RLenT - 1>{ lhs, rhs };
}

// Literal() + "" + ""
template<typename T, size_t LLenT, size_t RLenT>
constexpr ConcatLiterals<T, LLenT + RLenT - 1> operator+( ConcatLiterals<T, LLenT> const& lhs, T const ( &rhs )[RLenT] )
{
	return ConcatLiterals<T, LLenT + RLenT - 1>{ lhs.kResult, rhs };
}

///////////////////////////////////////////////////////////////////////////////
}
