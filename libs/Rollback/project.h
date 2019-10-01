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
