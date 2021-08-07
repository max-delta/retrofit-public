#pragma once


namespace RF::platform {
///////////////////////////////////////////////////////////////////////////////

// Readable, writable, but not executable
// NOTE: Implementation-defined, requires additional linkage to a platform
void* AcquireLargeExclusiveDataRegion( size_t size, char const* label );
void ReleaseLargeExclusiveDataRegion( void* address );

// Readable, writable, but not executable
// NOTE: Implementation-defined, requires additional linkage to a platform
void const* AcquireLargeDynamicHeapHandle( size_t maxSize, char const* label, bool threadSafe );
void ReleaseLargeDynamicHeapHandle( void const* heapHandle );
void* AllocateInLargeDynamicHeap( void const* heapHandle, size_t size );
void DeleteInLargeDynamicHeap( void const* heapHandle, void* address );
size_t QueryLargeDynamicHeapSize( void const* heapHandle );

///////////////////////////////////////////////////////////////////////////////
}
