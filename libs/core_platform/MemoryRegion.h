#pragma once


namespace RF { namespace platform {
///////////////////////////////////////////////////////////////////////////////

// Readable, writable, but not executable
// NOTE: Implementation-defined, requires additional linkage to a platform
void* AcquireLargeExclusiveDataRegion( size_t size, char const* label );
void ReleaseLargeExclusiveDataRegion( void* address );

///////////////////////////////////////////////////////////////////////////////
}}
