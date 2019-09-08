#pragma once

namespace RF { namespace hook {
///////////////////////////////////////////////////////////////////////////////

// If new is hooked, these must be present during the link step
void* GlobalObjectAllocate( size_t size, size_t align ) noexcept;
void GlobalObjectDeallocate( void* ptr ) noexcept;

///////////////////////////////////////////////////////////////////////////////
}}
