#pragma once

#include "rftl/string_view"


namespace RF::unicode {
///////////////////////////////////////////////////////////////////////////////

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
