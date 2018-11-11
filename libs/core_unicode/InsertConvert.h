#pragma once


namespace RF { namespace unicode {
///////////////////////////////////////////////////////////////////////////////

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT& insertIterator );
template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT& insertIterator );

template<typename InserterT> void ConvertToASCII( char const* source, size_t numBytes, InserterT& insertIterator );
template<typename InserterT> void ConvertToASCII( char16_t const* source, size_t numPairs, InserterT& insertIterator );
template<typename InserterT> void ConvertToASCII( char32_t const* source, size_t numCodePoints, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf8( char const* source, size_t numBytes, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf8( char16_t const* source, size_t numPairs, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf8( char32_t const* source, size_t numCodePoints, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf16( char const* source, size_t numBytes, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf16( char16_t const* source, size_t numPairs, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf16( char32_t const* source, size_t numCodePoints, InserterT& insertIterator );

template<typename InserterT> void ConvertToUtf32( char const* source, size_t numBytes, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf32( char16_t const* source, size_t numPairs, InserterT& insertIterator );
template<typename InserterT> void ConvertToUtf32( char32_t const* source, size_t numCodePoints, InserterT& insertIterator );

template<typename InserterT> void ConvertSingleUtf32ToUtf8( char32_t codePoint, InserterT&& insertIterator );
template<typename InserterT> void ConvertSingleUtf32ToUtf16( char32_t codePoint, InserterT&& insertIterator );

template<typename InserterT> void ConvertToASCII( char const* source, size_t numBytes, InserterT&& insertIterator );
template<typename InserterT> void ConvertToASCII( char16_t const* source, size_t numPairs, InserterT&& insertIterator );
template<typename InserterT> void ConvertToASCII( char32_t const* source, size_t numCodePoints, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf8( char const* source, size_t numBytes, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf8( char16_t const* source, size_t numPairs, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf8( char32_t const* source, size_t numCodePoints, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf16( char const* source, size_t numBytes, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf16( char16_t const* source, size_t numPairs, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf16( char32_t const* source, size_t numCodePoints, InserterT&& insertIterator );

template<typename InserterT> void ConvertToUtf32( char const* source, size_t numBytes, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf32( char16_t const* source, size_t numPairs, InserterT&& insertIterator );
template<typename InserterT> void ConvertToUtf32( char32_t const* source, size_t numCodePoints, InserterT&& insertIterator );

///////////////////////////////////////////////////////////////////////////////
}}

#include "InsertConvert.inl"
