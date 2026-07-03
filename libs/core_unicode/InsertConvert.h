#pragma once

#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

// Writes to an STL-style input iterator, by continously calling the assignment
//  operator until there are no more characters left in the source string
// NOTE: Nulls are handled like any other character, and are given no special
//  meaning during process, so will not act like terminators
// NOTE: No additional null terminator is added after the final insert

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT& insertIterator );
template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT& insertIterator );

template<typename InserterT> void ConvertToASCII( rftl::string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u8string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u16string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u32string_view source, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf8( rftl::string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u8string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u16string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u32string_view source, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf16( rftl::string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u8string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u16string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u32string_view source, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf32( rftl::string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u8string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u16string_view source, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u32string_view source, InserterT& insertIterator );

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT&& insertIterator );
template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT&& insertIterator );

template<typename InserterT> void ConvertToASCII( rftl::string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u8string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u16string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToASCII( rftl::u32string_view source, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf8( rftl::string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u8string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u16string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf8( rftl::u32string_view source, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf16( rftl::string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u8string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u16string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf16( rftl::u32string_view source, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf32( rftl::string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u8string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u16string_view source, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf32( rftl::u32string_view source, InserterT&& insertIterator );

///////////////////////////////////////////////////////////////////////////////
}

#include "InsertConvert.inl"
