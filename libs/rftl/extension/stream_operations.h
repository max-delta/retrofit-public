#pragma once
#include "rftl/iosfwd"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Like fseek(...)
template<typename Stream>
bool stream_seek_abs( Stream& source, size_t position );

// Like fread(...)
template<typename Stream>
size_t stream_read( void* dest, size_t count, Stream& source );

///////////////////////////////////////////////////////////////////////////////
}

#include "stream_operations.inl"
