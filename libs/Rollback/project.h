#pragma once

#ifndef RF_NO_SHARED_EXPORTS
#ifdef ROLLBACK_EXPORTS
#define ROLLBACK_API __declspec( dllexport )
#else
#define ROLLBACK_API __declspec( dllimport )
#endif
#else
#define ROLLBACK_API
#endif

struct RFTAG_ROLLBACK_SHARED_STATE
{
	static constexpr char const kIdentifier[] = "ROLLBACK_SHARED_STATE";
};
struct RFTAG_ROLLBACK_PRIVATE_STATE
{
	static constexpr char const kIdentifier[] = "ROLLBACK_PRIVATE_STATE";
};
struct RFTAG_ROLLBACK_SNAPSHOTS
{
	static constexpr char const kIdentifier[] = "ROLLBACK_SNAPSHOTS";
};
